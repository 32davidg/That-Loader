#include "../include/bootutils.h"


// Start a watchdog timer
//It works by continuously counting down from a specified time interval
// and if the software fails to reset the timer before it reaches zero
// the watchdog timer generates a system reset
void EnableWatchdogTimer(uintn_t seconds)
{
    efi_status_t status = BS->SetWatchdogTimer(seconds, 0, 0, NULL);
    if (EFI_ERROR(status))
    {
        Log(LL_WARNING, status, "Failed to set watchdog timeout to %d seconds.", seconds);
    }
}