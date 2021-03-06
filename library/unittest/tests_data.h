#pragma once

#include <util/generic/noncopyable.h>
#include <util/generic/ptr.h>
#include <util/folder/path.h>
#include <util/generic/string.h>
#include <util/system/file_lock.h>
#include <util/system/mutex.h>
#include <util/network/sock.h>

class TInet6StreamSocket;

TString GetArcadiaTestsData();
TString GetWorkPath();
TFsPath GetYaPath();
TFsPath GetOutputPath();

// set two options: SO_REUSEADDR and SO_REUSEPORT, both are required for
// correct implementation of TPortManager because of different operating systems
// incompatibility: singe SO_REUSEADDR is enough for Linux, but not enough for Darwin
template <class TSocketType>
void SetReuseAddressAndPort(const TSocketType& sock) {
    const int retAddr = SetSockOpt(sock, SOL_SOCKET, SO_REUSEADDR, 1);
    if (retAddr < 0) {
        ythrow yexception() << "can't set SO_REUSEADDR: " << LastSystemErrorText(-retAddr);
    }

#ifdef SO_REUSEPORT
    const int retPort = SetSockOpt(sock, SOL_SOCKET, SO_REUSEPORT, 1);
    if (retPort < 0) {
        ythrow yexception() << "can't set SO_REUSEPORT: " << LastSystemErrorText(-retPort);
    }
#endif
}

class TPortManager: public TNonCopyable {
public:
    TPortManager();
    ~TPortManager();

    // Gets free TCP port
    ui16 GetPort(ui16 port = 0);

    // Gets free TCP port
    ui16 GetTcpPort(ui16 port = 0);

    // Gets free UDP port
    ui16 GetUdpPort(ui16 port = 0);

    // Gets one free port for use in both TCP and UDP protocols
    ui16 GetTcpAndUdpPort(ui16 port = 0);

private:
    class TPortManagerImpl;
    THolder<TPortManagerImpl> Impl_;
};

class TPortsRangeManager {
    class TPortGuard {
    public:
        using TPtr = TSimpleSharedPtr<TPortGuard>;

        TPortGuard(const TFsPath& root, const ui16 port);

        bool IsLocked() const;

        ui16 GetPort() const;

        ~TPortGuard();

    private:
        TFsPath Path;
        ui16 Port;
        TFileLock Lock;
        TSimpleSharedPtr<TInet6StreamSocket> Socket;
        bool Locked = false;
    };

public:
    TPortsRangeManager(const TString& syncDir);

    ui16 GetPortsRange(const ui16 startPort, const ui16 range);

private:
    TFsPath WorkDir;
    TVector<TPortGuard::TPtr> ReservedPorts;
    TMutex Lock;
};
