#include "WowConnectionNet.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: 1491f4aa auto
// OFFSET: 0x00469060
static uint32_t STORMAPI NetworkThreadProc(void* param) {
    WowConnectionNet* network = static_cast<WowConnectionNet*>(param);
    network->PlatformRun();
    network->m_stopEvent.Set();

    return 0;
}

// VERIFY: d0e3b08a auto
// OFFSET: 0x00469300
static uint32_t STORMAPI WorkerThreadProc(void* param) {
    WowConnectionNet::WORKER* worker = static_cast<WowConnectionNet::WORKER*>(param);
    worker->m_owner->RunWorker(worker->m_index);

    return 0;
}

// INLINED
WowConnectionNet::WORKER::WORKER() : m_event(0, 0) {
}

// VERIFY: 87918bed manual
// OFFSET: 0x00469320
void WowConnectionNet::Start() {
    char name[32];
    uint32_t i = 0;

    for (; static_cast<int32_t>(i) < this->m_numWorkers; i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_index = i;
        worker.m_connection = nullptr;
        worker.m_quit = false;
        worker.m_owner = this;
        worker.m_lastTime = OsGetAsyncTimeMs();
        SStrPrintf(name, sizeof(name), "NetThread %d", i);
        SThread::Create(WorkerThreadProc, &worker, worker.m_thread, name, 0);
    }

    for (; i < sizeof(this->m_workers) / sizeof(this->m_workers[0]); i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_index = 0;
        worker.m_connection = nullptr;
        worker.m_quit = true;
        worker.m_owner = this;
    }

    SStrPrintf(name, sizeof(name), "Network");
    this->m_networkStarted = false;
    SThread::Create(NetworkThreadProc, this, this->m_thread, name, 0);

    while (!this->m_networkStarted) {
        OsSleep(100);
    }
}

// VERIFY: 6dcda1a1 auto
// OFFSET: 0x00469160
void WowConnectionNet::RunWorker(int32_t index) {
    if (this->m_threadInit) {
        this->m_threadInit();
    }

    this->m_workerSemaphore.Signal(1);

    WowConnectionNet::WORKER& worker = this->m_workers[index];

    while (true) {
        do {
            worker.m_lastTime = OsGetAsyncTimeMs();
        } while (worker.m_event.Wait(1000));

        if (worker.m_quit) {
            break;
        }

        uint32_t serviceFlags = worker.m_connection->m_serviceFlags;
        worker.m_connection->m_serviceFlags = 0;
        this->Service(worker.m_connection, serviceFlags);

        worker.m_lock.Enter();
        InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&worker.m_connection->unk_0130), -1);
        worker.m_connection->Release();
        worker.m_connection = nullptr;
        worker.m_lock.Leave();

        this->m_workerSemaphore.Signal(1);
        this->PlatformWorkerReady();
    }
}

// VERIFY: da8d942b manual
// OFFSET: 0x004695F0
WowConnectionNet::WowConnectionNet(int32_t numThreads, void (*threadInit)()) : m_stopEvent(1, 0), m_workerSemaphore(0, numThreads), m_serviceSemaphore(0, 0x7FFFFFFF) {
    this->m_numWorkers = numThreads;
    this->m_threadInit = threadInit;
    this->m_stop = false;
}
