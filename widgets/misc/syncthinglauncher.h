#ifndef SYNCTHINGWIDGETS_SYNCTHINGLAUNCHER_H
#define SYNCTHINGWIDGETS_SYNCTHINGLAUNCHER_H

#include "../global.h"

#include "../../connector/syncthingprocess.h"

#include <QFuture>

namespace LibSyncthing {
struct RuntimeOptions;
}

namespace Data {

class SYNCTHINGWIDGETS_EXPORT SyncthingLauncher : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(ChronoUtilities::DateTime activeSince READ activeSince)
    Q_PROPERTY(bool manuallyStopped READ isManuallyStopped)

public:
    explicit SyncthingLauncher(QObject *parent = nullptr);

    bool isRunning() const;
    ChronoUtilities::DateTime activeSince() const;
    bool isActiveFor(unsigned int atLeastSeconds) const;
    bool isManuallyStopped() const;
    static bool isLibSyncthingAvailable();
    static SyncthingLauncher *mainInstance();
    static void setMainInstance(SyncthingLauncher *mainInstance);

Q_SIGNALS:
    void confirmKill();
    void runningChanged(bool isRunning);
    void outputAvailable(const QByteArray &data);
    void exited(int exitCode, QProcess::ExitStatus exitStatus);

public Q_SLOTS:
    void launch(const QString &cmd);
    void launch(const LibSyncthing::RuntimeOptions &runtimeOptions);
    void terminate();
    void kill();

private Q_SLOTS:
    void handleProcessReadyRead();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void runLibSyncthing(const LibSyncthing::RuntimeOptions &runtimeOptions);

private:
    SyncthingProcess m_process;
    QFuture<void> m_future;
    ChronoUtilities::DateTime m_futureStarted;
    bool m_manuallyStopped;
    static SyncthingLauncher *s_mainInstance;
};

inline bool SyncthingLauncher::isRunning() const
{
    return m_process.isRunning() || m_future.isRunning();
}

inline ChronoUtilities::DateTime SyncthingLauncher::activeSince() const
{
    if (m_process.isRunning()) {
        return m_process.activeSince();
    } else if (m_future.isRunning()) {
        return m_futureStarted;
    }
    return ChronoUtilities::DateTime();
}

inline bool SyncthingLauncher::isActiveFor(unsigned int atLeastSeconds) const
{
    const auto activeSince(this->activeSince());
    return !activeSince.isNull() && (ChronoUtilities::DateTime::gmtNow() - activeSince).totalSeconds() > atLeastSeconds;
}

inline bool SyncthingLauncher::isManuallyStopped() const
{
    return m_manuallyStopped;
}

inline SyncthingLauncher *SyncthingLauncher::mainInstance()
{
    return s_mainInstance;
}

inline void SyncthingLauncher::setMainInstance(SyncthingLauncher *mainInstance)
{
    if ((s_mainInstance = mainInstance) && !SyncthingProcess::mainInstance()) {
        SyncthingProcess::setMainInstance(&mainInstance->m_process);
    }
}

SyncthingLauncher SYNCTHINGWIDGETS_EXPORT &syncthingLauncher();

} // namespace Data

#endif // SYNCTHINGWIDGETS_SYNCTHINGLAUNCHER_H