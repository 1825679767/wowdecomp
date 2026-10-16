"""Fetch hash-pinned official build headers and AMD64 DXBC signer. Never installs a client."""
from pathlib import Path, PurePosixPath
import argparse
import hashlib
import json
import urllib.request
import zipfile

PACKAGES = [
    ('wdk-x64-10.0.26100.6584.nupkg',
     'https://api.nuget.org/v3-flatcontainer/microsoft.windows.wdk.x64/10.0.26100.6584/microsoft.windows.wdk.x64.10.0.26100.6584.nupkg',
     'c393d03dfb640b5c92f546b32f6770ef68cd3aaf691956e7d66d8e2c28a1b55e', 'wdk'),
    ('dxbcsigner-1.1.0.nupkg',
     'https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.dxbcsigner/1.1.0/microsoft.direct3d.dxbcsigner.1.1.0.nupkg',
     '21785b2e393573964bae6693984c07bd918a9a71edbf9b977545f739eb5b14b9', 'dxbcsigner'),
]


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    root = parser.parse_args().output.resolve()
    root.mkdir(parents=True, exist_ok=True)
    records = []
    for name, url, expected, folder in PACKAGES:
        package = root / name
        if not package.exists():
            partial = package.with_suffix('.partial')
            with urllib.request.urlopen(url, timeout=120) as source, partial.open('xb') as dest:
                while chunk := source.read(1024 * 1024):
                    dest.write(chunk)
            if digest(partial) != expected:
                raise RuntimeError('download hash mismatch; retained ' + str(partial))
            partial.rename(package)
        if digest(package) != expected:
            raise RuntimeError('package hash mismatch: ' + str(package))
        with zipfile.ZipFile(package) as archive:
            for info in archive.infolist():
                rel = PurePosixPath(info.filename)
                if rel.is_absolute() or '..' in rel.parts or ':' in info.filename or '\\' in info.filename:
                    raise RuntimeError('unsafe package path')
                if info.is_dir():
                    continue
                if folder == 'wdk' and '/Include/' not in info.filename and info.filename != 'LICENSE.txt':
                    continue
                data = archive.read(info)
                dst = root / folder / Path(*rel.parts)
                if dst.exists():
                    if dst.read_bytes() != data:
                        raise RuntimeError('existing dependency differs: ' + str(dst))
                else:
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    dst.write_bytes(data)
                records.append({'path': dst.as_posix(), 'size': len(data), 'sha256': hashlib.sha256(data).hexdigest()})
    manifest = root / 'pinned-dependencies.json'
    manifest.write_text(json.dumps({'packages': PACKAGES, 'files': records}, indent=2) + '\n', encoding='utf-8')
    print('WDK:', root / 'wdk/c/Include/10.0.26100.0')
    print('Signer:', root / 'dxbcsigner/build/native/bin/x64/dxbcSigner.dll')
    print('Manifest:', manifest)


if __name__ == '__main__':
    main()
