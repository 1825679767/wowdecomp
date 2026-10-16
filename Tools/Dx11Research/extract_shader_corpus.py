"""Extract a hashed stock shader corpus using an explicit archive precedence fixture."""
from pathlib import Path, PureWindowsPath
import argparse
import hashlib
import json
import os
import subprocess

ORDER = ['common.MPQ', 'common-2.MPQ', 'expansion.MPQ', 'lichking.MPQ', 'patch.MPQ', 'patch-2.MPQ', 'patch-3.MPQ']
PROFILES = {'vertex': ['vs_3_0', 'vs_2_0', 'vs_1_1'], 'pixel': ['ps_3_0', 'ps_2_0', 'ps_1_4', 'ps_1_1']}


def identity(path):
    before = path.stat()
    with path.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    after = path.stat()
    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
        raise RuntimeError('archive changed while hashing: ' + str(path))
    return {'path': path.as_posix(), 'sha256': digest, 'size': before.st_size}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive-root', type=Path, required=True)
    parser.add_argument('--probe', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    root = args.output.resolve()
    root.mkdir(exist_ok=False, parents=True)
    env = {k.upper(): v for k, v in os.environ.items()}
    candidates, archives, superseded = {}, [], []
    for name in ORDER:
        archive = args.archive_root / name
        listing = root / 'listfiles' / (name + '.txt')
        listing.parent.mkdir(exist_ok=True)
        subprocess.run([str(args.probe), str(archive), '--extract', '(listfile)', str(listing)], check=True, env=env)
        record = identity(archive)
        archives.append(record)
        for member in listing.read_text(encoding='utf-8-sig', errors='replace').splitlines():
            rel = PureWindowsPath(member)
            parts = rel.parts
            if len(parts) != 4 or parts[0].lower() != 'shaders' or parts[1].lower() not in PROFILES or rel.suffix.lower() != '.bls':
                continue
            if rel.is_absolute() or '..' in parts or ':' in member:
                raise RuntimeError('unsafe shader member')
            if parts[2].lower() not in PROFILES[parts[1].lower()]:
                continue
            key = member.replace('/', '\\').lower()
            if key in candidates:
                superseded.append({'member': key, 'before': candidates[key]['archive'], 'after': archive.as_posix()})
            candidates[key] = {'archive': archive.as_posix(), 'member': member}
        print('indexed ' + name + ': ' + str(len(candidates)) + ' candidate members', flush=True)
    families = {}
    for key, item in candidates.items():
        p = PureWindowsPath(key)
        family = (p.parts[1], p.name)
        rank = PROFILES[p.parts[1]].index(p.parts[2])
        if family not in families or rank < families[family][0]:
            families[family] = rank, key, item
    outputs = []
    for key, item in sorted(candidates.items()):
        path = root / 'members' / Path(*PureWindowsPath(key).parts)
        path.parent.mkdir(exist_ok=True, parents=True)
        subprocess.run([str(args.probe), item['archive'], '--extract', item['member'], str(path)], check=True, env=env)
        outputs.append(dict(item, output=identity(path), canonical_member=key))
    manifest = {'schema': 1, 'scope': 'Explicit stock seven-archive precedence fixture, low to high; not an installed-client mounted precedence claim',
                'archives': archives, 'archive_order': ORDER, 'profiles': PROFILES,
                'superseded_members': superseded, 'selected': outputs,
                'preferred_profile_candidates': [r[1] for r in sorted(families.values(), key=lambda r: r[1])]}
    (root / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'profile_members': len(outputs), 'logical_families': len(families), 'bytes': sum(r['output']['size'] for r in outputs)}, indent=2), flush=True)


if __name__ == '__main__':
    main()
