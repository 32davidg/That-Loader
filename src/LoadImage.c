#include "../include/LoadImage.h"
#include "../include/logs.h"
#include "../include/bootutils.h"


/*
*   
*/
void ChainloadImage(char_t* path, char_t* args)
{
    // A device handle must be passed to the loaded image protocol because of the way we call LoadImage
    efi_handle_t devHandle = GetFileDeviceHandle(path);
    if(devHandle == NULL)
    {
        Log(LL_ERROR, "Unable to find device handle in chainloading process '%s'.", path);
        return;
    }

    efi_device_path_t* devPath = NULL;
    efi_guid_t devPathGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    // this gives me the protocol of the device (file s)
    efi_status_t status = BS->HandleProtocol(devHandle, &devPathGuid, (void**)&devPath);
    if(EFI_ERROR(status))
    {
        Log(LL_ERROR, status, "Failed to handle device path '%s'.", devPath);
        return;
    }

    // Read the file into a buffer
    uintn_t imgFileSize = 0;
    char_t* imgData = GetFileContent(path, &imgFileSize);
    if(imgData == NULL)
    {
        Log(LL_ERROR, 0, "Failed to read file '%s' for chainloading.", path);
        return;
    }

    //Load the image
    efi_handle_t imgHandle;
    status = BS->LoadImage(FALSE, IM, devPath, imgData, imgFileSize, &imgHandle);
    if(EFI_ERROR(status))
    {
        Log(LL_ERROR, 0, "Failed to load the image '%s' for chainloading.", path);
        //goto cleanup;
    }

    efi_guid_t loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    efi_loaded_image_protocol_t* imgProtocol = NULL;
    status = BS->HandleProtocol(imgHandle, &loadedImageGuid, (void**)&imgProtocol);
    if(EFI_ERROR(status))
    {
        Log(LL_ERROR, 0, "Failed to get loaded image protocol when passing arguments.");
        //goto cleanup;
    }
    else
    {
        //add args to image, if any
        if (args != NULL)
        {
            imgProtocol->LoadOptions = StringToWideString(args);
            imgProtocol->LoadOptionsSize = (strlen(args) + 1) * sizeof(wchar_t);
        }
        imgProtocol->DeviceHandle = devHandle;
    }

    Log(LL_INFO, 0, "Chainloading the image... '%s'", path);
    status = BS->StartImage(imgHandle, NULL, NULL);
    if(EFI_ERROR(status))
    {
        Log(LL_ERROR, 0, "Failed to start the image '%s'", path);
    }

cleanup:
// if chainloading fails, we have to clean things up for the next booting
// aaaaaaaand we gotta take care of mem leaks
    if(args != NULL)
    {
        free(imgProtocol->LoadOptions);
    }
    free(imgData);




}