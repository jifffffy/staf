/*****************************************************************************/
/* Software Testing Automation Framework (STAF)                              */
/* (C) Copyright IBM Corp. 2001                                              */
/*                                                                           */
/* This software is licensed under the Eclipse Public License (EPL) V1.0.    */
/*****************************************************************************/

#include <pthread.h>
#include "STAFOSTypes.h"
#include "STAFMutexSem.h"


struct STAFMutexSemImplementation
{
    bool fIsOwned;
    pthread_mutex_t fMutex;
    pthread_cond_t fCond;
#if !defined(STAF_OS_NAME_HPUX) && !defined(STAF_OS_NAME_MACOSX) && \
    !defined(STAF_OS_NAME_ZOS)
    // On all operating systems other than HP-UX, Mac OS X, and z/OS (which
    // don't support monotonic clocks), need a clock attribute to specify to
    // use a monotonic clock to measure the timeout service of
    // pthread_cond_timedwait().
    pthread_condattr_t fClockAttr;
#endif    
};


STAFRC_t STAFMutexSemConstruct(STAFMutexSem_t *pMutex, 
                               const char *name,
                               unsigned int *osRC)
{
    STAFRC_t rc = kSTAFOk;

    if (pMutex == 0) return kSTAFInvalidObject;

    try
    {
        // We don't currently support named mutexes
        if (name != 0) return kSTAFInvalidParm;

        *pMutex = new STAFMutexSemImplementation;
        STAFMutexSemImplementation &mutexSem = **pMutex;
        mutexSem.fIsOwned = false;

        int rc2 = pthread_mutex_init(&mutexSem.fMutex, 0);

        if (rc2 != 0)
        {
            if (osRC) *osRC = rc2;
            delete *pMutex;
            return kSTAFBaseOSError;
        }

#if defined(STAF_OS_NAME_HPUX) || defined(STAF_OS_NAME_MACOSX) || \
    defined(STAF_OS_NAME_ZOS)
        // On HP-UX, Mac OS X, and z/OS, use a real-time clock since these
        // operating systems don't provide support for monotonic clocks.
            
        rc2 = pthread_cond_init(&mutexSem.fCond, 0);

        if (rc2 != 0)
        {
            if (osRC) *osRC = rc2;
            pthread_mutex_destroy(&mutexSem.fMutex);
            delete *pMutex;
            return kSTAFBaseOSError;
        }
#else
        // Use a monotonic clock since it is better for measuring elapsed
        // time because it is not effected by system clock changes.
        
        int rc3 = pthread_condattr_init(&mutexSem.fClockAttr);
        
        if (rc3 != 0)
        {
            if (osRC) *osRC = rc3;
            pthread_mutex_destroy(&mutexSem.fMutex);
            delete *pMutex;
            return kSTAFBaseOSError;
        }

        int rc4 = pthread_condattr_setclock(&mutexSem.fClockAttr,
                                            CLOCK_MONOTONIC);
        if (rc4 != 0)
        {
            if (osRC) *osRC = rc4;
            pthread_condattr_destroy(&mutexSem.fClockAttr);
            pthread_mutex_destroy(&mutexSem.fMutex);
            delete *pMutex;
            return kSTAFBaseOSError;
        }

        int rc5 = pthread_cond_init(&mutexSem.fCond, &mutexSem.fClockAttr);
        
        if (rc5 != 0)
        {
            if (osRC) *osRC = rc5;
            pthread_condattr_destroy(&mutexSem.fClockAttr);
            pthread_mutex_destroy(&mutexSem.fMutex);
            delete *pMutex;
            return kSTAFBaseOSError;
        }
#endif        
    }
    catch (...)
    { rc = kSTAFUnknownError; if (osRC) *osRC = 0xFFFFFFFF; }

    return rc;
}


STAFRC_t STAFMutexSemRequest(STAFMutexSem_t mutex,
                             unsigned int timeout, unsigned int *osRC)
{
    STAFRC_t rc = kSTAFOk;
    unsigned int rc2 = 0;

    if (mutex == 0) return kSTAFInvalidObject;

#if defined(STAF_OS_NAME_HPUX) || defined(STAF_OS_NAME_MACOSX) || \
    defined(STAF_OS_NAME_ZOS)
    // On HP-UX, Mac OS X, and z/OS, use a real-time clock since these
    // operating systems don't provide support for monotonic clocks.
    
    struct timeval now; // Time when we started

    if (timeout != STAF_MUTEX_SEM_INDEFINITE_WAIT)
    {
        // Get current time

        // Use gettimeofday() to get the actual time with a 0.01 second
        // resolution. A timeval struct is passed to it.  A timeval has
        // two components:  tv_sec is the time in seconds and tv_usec is
        // the number of microseconds. Although the units are microseconds,
        // the value may not be that accurate on some operating systems.

        if (gettimeofday(&now, NULL) != 0)
        {
            if (osRC) *osRC = errno;
            return kSTAFBaseOSError;
        }
    }
#else
    // Use a monotonic clock since it is better for measuring elapsed
    // time because it is not effected by system clock changes.        
    
    struct timespec now; // Time when we started
    
    if (timeout != STAF_MUTEX_SEM_INDEFINITE_WAIT)
    {
        // Get current time

        // Use clock_gettime() to get the actual time with a nanosecond
        // resolution. A timespec struct is passed to it.  A timespec has
        // two components:  tv_sec is the time in seconds and tv_nsec is
        // the number of nanoseconds. Although the units are nanoseconds,
        // the value may not be that accurate on some operating systems.
        
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        {
            if (osRC) *osRC = errno;
            return kSTAFBaseOSError;
        }
    }
#endif    

    STAFMutexSemImplementation &mutexSem = *mutex;

    if ((rc2 = pthread_mutex_lock(&mutexSem.fMutex)) != 0)
    {
        if (osRC) *osRC = rc2;
        return kSTAFBaseOSError;
    }

    if (mutexSem.fIsOwned == false)
    {
        mutexSem.fIsOwned = true;
        pthread_mutex_unlock(&mutexSem.fMutex);
        return kSTAFOk;
    }

    struct timespec absTime;

    if (timeout != STAF_MUTEX_SEM_INDEFINITE_WAIT)
    {
        // Prepare the maximum time to block the thread, expressed as
        // an absolute time in a timespec structure, absTime
        // (consisting of seconds and nanoseconds).

        // To do this, add the specified timeout (in milliseconds) to
        // the starting time contained in the "now" timeval structure
        // (consisting of seconds and microseconds).
        // Notes:
        // 1) 1 microsecond == 1000 milliseconds
        // 2) 1 nanosecond == 1000 microseconds (1000000 microseconds)
        // 3) 1 second == 1000000000 nanoseconds
        // 4) A timespec's tv_nsec field (nanoseconds) must be less
        //    than 1000000000 (one billion) or else this will cause a
        //    pthread_cond_timedwait to return EINVAL (Invalid Value).

#if defined(STAF_OS_NAME_HPUX) || defined(STAF_OS_NAME_MACOSX) || \
    defined(STAF_OS_NAME_ZOS)        
        absTime.tv_sec = now.tv_sec + (timeout / 1000) +
            ((now.tv_usec + ((timeout % 1000) * 1000)) / 1000000);
        absTime.tv_nsec = 
            ((now.tv_usec + ((timeout % 1000) * 1000)) % 1000000) * 1000;
#else
        absTime.tv_sec = now.tv_sec + (timeout / 1000) +
            ((now.tv_nsec + ((timeout % 1000) * 1000000)) / 1000000000);
        absTime.tv_nsec = 
            ((now.tv_nsec + ((timeout % 1000) * 1000000)) % 1000000000);
#endif            
    }

    rc2 = EINTR;

    while ((rc2 == EINTR) ||
           ((rc2 == 0) && (mutexSem.fIsOwned == true)) ||
           ((timeout == STAF_MUTEX_SEM_INDEFINITE_WAIT) &&
            (mutexSem.fIsOwned == true)))
    {
        if (timeout == STAF_MUTEX_SEM_INDEFINITE_WAIT)
        {
            rc2 = pthread_cond_wait(&mutexSem.fCond, &mutexSem.fMutex);
        }
        else
        {
            rc2 = pthread_cond_timedwait(&mutexSem.fCond, &mutexSem.fMutex,
                                         &absTime);
        }
    }

    // XXX: Hack Hack!  z/OS returns EAGAIN instead of ETIMEDOUT
    //      This is not how this will ultimately be coded.  This will
    //      probably require the same fix as for the Solaris 9 bug
    
    if ((rc2 == ETIMEDOUT) || (rc2 == EAGAIN))
    {
        rc = kSTAFTimeout;
    }
    else if (rc2 != 0)
    {
        rc = kSTAFBaseOSError;
        if (osRC) *osRC = rc2;
    }
    else
    {
        mutexSem.fIsOwned = true;
    }

    pthread_mutex_unlock(&mutexSem.fMutex);

    return rc;
}


STAFRC_t STAFMutexSemRelease(STAFMutexSem_t mutex, unsigned int *osRC)
{
    STAFRC_t rc = kSTAFOk;
    unsigned int rc2 = 0;

    if (mutex == 0) return kSTAFInvalidObject;

    STAFMutexSemImplementation &mutexSem = *mutex;

    if ((rc2 = pthread_mutex_lock(&mutexSem.fMutex)) != 0)
    {
        if (osRC) *osRC = rc2;
        return kSTAFBaseOSError;
    }

    if (mutexSem.fIsOwned == false)
    {
        /* Do Nothing */
    }
    else if ((rc2 = pthread_cond_signal(&mutexSem.fCond)) != 0)
    {
        if (osRC) *osRC = errno;
        rc = kSTAFBaseOSError;
    }
    else
    {
        mutexSem.fIsOwned = false;
    }

    pthread_mutex_unlock(&mutexSem.fMutex);

    // XXX: Give another thread a chance to get the semaphore.  Should we
    //      be doing this? NO!!!
    // sleep(0);

    return rc;
}


STAFRC_t STAFMutexSemDestruct(STAFMutexSem_t *pMutex, unsigned int *osRC)
{
    STAFRC_t rc = kSTAFOk;
    unsigned int rc2 = 0;

    if (pMutex == 0) return kSTAFInvalidObject;

    STAFMutexSemImplementation &mutexSem = **pMutex;

    if ((rc2 = pthread_cond_destroy(&mutexSem.fCond)) != 0)
    {
        rc = kSTAFBaseOSError;
        if (osRC) *osRC = rc2;
    }

#if !defined(STAF_OS_NAME_HPUX) && !defined(STAF_OS_NAME_MACOSX) && \
    !defined(STAF_OS_NAME_ZOS)
    // Destroy the clock attribute for OSes that support a monotonic clock     
    if ((rc2 = pthread_condattr_destroy(&mutexSem.fClockAttr)) != 0)
    {
        rc = kSTAFBaseOSError;
        if (osRC) *osRC = rc2;
    }
#endif

    if ((rc2 = pthread_mutex_destroy(&mutexSem.fMutex)) != 0)
    {
        rc = kSTAFBaseOSError;
        if (osRC) *osRC = rc2;
    }

    delete *pMutex;
    *pMutex = 0;

    return rc;
}
