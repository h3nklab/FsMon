/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Copyright (C) 2020 H3nklab Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    File name: FsMon.c
    Contact: hdmih@yahoo.com
    Created: 04-07-2020
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <fltKernel.h>
#include <dontuse.h>
#include <ntstrsafe.h>

#include "struct.h"
#include "FsMon.h"
#include "utils.h"
#include "init.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER       g_FilterHandle;
ULONG_PTR         g_ulOperationStatusCtx = 1;
PFSMON_SETTINGS   g_pSettings = NULL;

PLIST_ENTRY       g_pVolumeMapHeader = NULL;
PFAST_MUTEX       g_pVolumeMapMutex = NULL;

DRIVER_INITIALIZE DriverEntry;

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, FsMonUnload)
#pragma alloc_text(PAGE, FsMonInstanceQueryTeardown)
#pragma alloc_text(PAGE, FsMonInstanceSetup)
#pragma alloc_text(PAGE, FsMonInstanceTeardownStart)
#pragma alloc_text(PAGE, FsMonInstanceTeardownComplete)
#pragma alloc_text(PAGE, FsMonGenerateFileName)
#pragma alloc_text(PAGE, FsMonGenerateDestinationFileName)
#pragma alloc_text(PAGE, FsMonCreatePreOp)
#pragma alloc_text(PAGE, FsMonCreatePostOp)
#pragma alloc_text(PAGE, FsMonReadPreOp)
#pragma alloc_text(PAGE, FsMonReadPostOp)
#pragma alloc_text(PAGE, FsMonWritePreOp)
#pragma alloc_text(PAGE, FsMonWritePostOp)
#pragma alloc_text(PAGE, FsMonCreateNamedPipePreOp)
#pragma alloc_text(PAGE, FsMonCreateNamedPipePostOp)
#pragma alloc_text(PAGE, FsMonClosePreOp)
#pragma alloc_text(PAGE, FsMonClosePostOp)
#pragma alloc_text(PAGE, FsMonQueryInfoPreOp)
#pragma alloc_text(PAGE, FsMonQueryInfoPostOp)
#pragma alloc_text(PAGE, FsMonSetInfoPreOp)
#pragma alloc_text(PAGE, FsMonSetInfoPostOp)
#pragma alloc_text(PAGE, FsMonQueryEaPreOp)
#pragma alloc_text(PAGE, FsMonQueryEaPostOp)
#pragma alloc_text(PAGE, FsMonSetEaPreOp)
#pragma alloc_text(PAGE, FsMonSetEaPostOp)
#pragma alloc_text(PAGE, FsMonFlushBuffersPreOp)
#pragma alloc_text(PAGE, FsMonFlushBuffersPostOp)
#pragma alloc_text(PAGE, FsMonQueryVolumeInfoPreOp)
#pragma alloc_text(PAGE, FsMonQueryVolumeInfoPostOp)
#pragma alloc_text(PAGE, FsMonSetVolumeInfoPreOp)
#pragma alloc_text(PAGE, FsMonSetVolumeInfoPostOp)
#pragma alloc_text(PAGE, FsMonDirCtrlPreOp)
#pragma alloc_text(PAGE, FsMonDirCtrlPostOp)
#pragma alloc_text(PAGE, FsMonFsCtrlPreOp)
#pragma alloc_text(PAGE, FsMonFsCtrlPostOp)
#pragma alloc_text(PAGE, FsMonDevCtrlPreOp)
#pragma alloc_text(PAGE, FsMonDevCtrlPostOp)
#pragma alloc_text(PAGE, FsMonIntDevCtrlPreOp)
#pragma alloc_text(PAGE, FsMonIntDevCtrlPostOp)
#pragma alloc_text(PAGE, FsMonShutdownPreOp)
#pragma alloc_text(PAGE, FsMonLockCtrlPreOp)
#pragma alloc_text(PAGE, FsMonLockCtrlPostOp)
#pragma alloc_text(PAGE, FsMonCleanupPreOp)
#pragma alloc_text(PAGE, FsMonCleanupPostOp)
#pragma alloc_text(PAGE, FsMonCreateMailSlotPreOp)
#pragma alloc_text(PAGE, FsMonCreateMailSlotPostOp)
#pragma alloc_text(PAGE, FsMonQuerySecurityPreOp)
#pragma alloc_text(PAGE, FsMonQuerySecurityPostOp)
#pragma alloc_text(PAGE, FsMonSetSecurityPreOp)
#pragma alloc_text(PAGE, FsMonSetSecurityPostOp)
#pragma alloc_text(PAGE, FsMonQueryQuotaPreOp)
#pragma alloc_text(PAGE, FsMonQueryQuotaPostOp)
#pragma alloc_text(PAGE, FsMonSetQuotaPreOp)
#pragma alloc_text(PAGE, FsMonSetQuotaPostOp)
#pragma alloc_text(PAGE, FsMonPnpPreOp)
#pragma alloc_text(PAGE, FsMonPnpPostOp)
#pragma alloc_text(PAGE, FsMonAcquireSectionSyncPreOp)
#pragma alloc_text(PAGE, FsMonAcquireSectionSyncPostOp)
#pragma alloc_text(PAGE, FsMonReleaseSectionSyncPreOp)
#pragma alloc_text(PAGE, FsMonReleaseSectionSyncPostOp)
#pragma alloc_text(PAGE, FsMonAcquireModWritePreOp)
#pragma alloc_text(PAGE, FsMonAcquireModWritePostOp)
#pragma alloc_text(PAGE, FsMonReleaseModWritePreOp)
#pragma alloc_text(PAGE, FsMonReleaseModWritePostOp)
#pragma alloc_text(PAGE, FsMonAcquireCcFlushPreOp)
#pragma alloc_text(PAGE, FsMonAcquireCcFlushPostOp)
#pragma alloc_text(PAGE, FsMonReleaseCcFlushPreOp)
#pragma alloc_text(PAGE, FsMonReleaseCcFlushPostOp)
#pragma alloc_text(PAGE, FsMonFastIoCheckPreOp)
#pragma alloc_text(PAGE, FsMonFastIoCheckPostOp)
#pragma alloc_text(PAGE, FsMonNetQueryOpenPreOp)
#pragma alloc_text(PAGE, FsMonNetQueryOpenPostOp)
#pragma alloc_text(PAGE, FsMonMdlReadPreOp)
#pragma alloc_text(PAGE, FsMonMdlReadPostOp)
#pragma alloc_text(PAGE, FsMonMdlReadCompletePreOp)
#pragma alloc_text(PAGE, FsMonMdlReadCompletePostOp)
#pragma alloc_text(PAGE, FsMonPrepareMdlWritePreOp)
#pragma alloc_text(PAGE, FsMonPrepareMdlWritePostOp)
#pragma alloc_text(PAGE, FsMonMdlWriteCompletePreOp)
#pragma alloc_text(PAGE, FsMonMdlWriteCompletePostOp)
#pragma alloc_text(PAGE, FsMonVolumeMountPreOp)
#pragma alloc_text(PAGE, FsMonVolumeMountPostOp)
#pragma alloc_text(PAGE, FsMonVolumeDismountPreOp)
#pragma alloc_text(PAGE, FsMonVolumeDismountPostOp)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

    { IRP_MJ_CREATE,
      FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
      FsMonCreatePreOp,
      FsMonCreatePostOp },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      FsMonCreateNamedPipePreOp,
      FsMonCreateNamedPipePostOp },

    { IRP_MJ_CLOSE,
      0,
      FsMonClosePreOp,
      FsMonClosePostOp },

    { IRP_MJ_READ,
      0,
      FsMonReadPreOp,
      FsMonReadPostOp },

    { IRP_MJ_WRITE,
      0,
      FsMonWritePreOp,
      FsMonWritePostOp },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      FsMonQueryInfoPreOp,
      FsMonQueryInfoPostOp },

    { IRP_MJ_SET_INFORMATION,
      0,
      FsMonSetInfoPreOp,
      FsMonSetInfoPostOp },

    { IRP_MJ_QUERY_EA,
      0,
      FsMonQueryEaPreOp,
      FsMonQueryEaPostOp },

    { IRP_MJ_SET_EA,
      0,
      FsMonSetEaPreOp,
      FsMonSetEaPostOp },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      FsMonFlushBuffersPreOp,
      FsMonFlushBuffersPostOp },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      FsMonQueryVolumeInfoPreOp,
      FsMonQueryVolumeInfoPostOp },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      FsMonSetVolumeInfoPreOp,
      FsMonSetVolumeInfoPostOp },

    { IRP_MJ_DIRECTORY_CONTROL,
      0,
      FsMonDirCtrlPreOp,
      FsMonDirCtrlPostOp },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      FsMonFsCtrlPreOp,
      FsMonFsCtrlPostOp },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      FsMonDevCtrlPreOp,
      FsMonDevCtrlPostOp },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      FsMonIntDevCtrlPreOp,
      FsMonIntDevCtrlPostOp },

    { IRP_MJ_SHUTDOWN,
      0,
      FsMonShutdownPreOp,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      FsMonLockCtrlPreOp,
      FsMonLockCtrlPostOp },

    { IRP_MJ_CLEANUP,
      0,
      FsMonCleanupPreOp,
      FsMonCleanupPostOp },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      FsMonCreateMailSlotPreOp,
      FsMonCreateMailSlotPostOp },

    { IRP_MJ_QUERY_SECURITY,
      0,
      FsMonQuerySecurityPreOp,
      FsMonQuerySecurityPostOp },

    { IRP_MJ_SET_SECURITY,
      0,
      FsMonSetSecurityPreOp,
      FsMonSetSecurityPostOp },

    { IRP_MJ_QUERY_QUOTA,
      0,
      FsMonQueryQuotaPreOp,
      FsMonQueryQuotaPostOp },

    { IRP_MJ_SET_QUOTA,
      0,
      FsMonSetQuotaPreOp,
      FsMonSetQuotaPostOp },

    { IRP_MJ_PNP,
      0,
      FsMonPnpPreOp,
      FsMonPnpPostOp },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsMonAcquireSectionSyncPreOp,
      FsMonAcquireSectionSyncPostOp },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsMonReleaseSectionSyncPreOp,
      FsMonReleaseSectionSyncPostOp },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      FsMonAcquireModWritePreOp,
      FsMonAcquireModWritePostOp },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      FsMonReleaseModWritePreOp,
      FsMonReleaseModWritePostOp },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      FsMonAcquireCcFlushPreOp,
      FsMonAcquireCcFlushPostOp },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      FsMonReleaseCcFlushPreOp,
      FsMonReleaseCcFlushPostOp },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      FsMonFastIoCheckPreOp,
      FsMonFastIoCheckPostOp },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
      FsMonNetQueryOpenPreOp,
      FsMonNetQueryOpenPostOp },

    { IRP_MJ_MDL_READ,
      0,
      FsMonMdlReadPreOp,
      FsMonMdlReadPostOp },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      FsMonMdlReadCompletePreOp,
      FsMonMdlReadCompletePostOp },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      FsMonPrepareMdlWritePreOp,
      FsMonPrepareMdlWritePostOp },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      FsMonMdlWriteCompletePreOp,
      FsMonMdlWriteCompletePostOp },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      FsMonVolumeMountPreOp,
      FsMonVolumeMountPostOp },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      FsMonVolumeDismountPreOp,
      FsMonVolumeDismountPostOp },

    { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),          //  Size
    FLT_REGISTRATION_VERSION,          //  Version
    0,                                 //  Flags
    NULL,                              //  Context
    Callbacks,                         //  Operation callbacks
    FsMonUnload,                       //  MiniFilterUnload
    FsMonInstanceSetup,                //  InstanceSetup
    FsMonInstanceQueryTeardown,        //  InstanceQueryTeardown
    FsMonInstanceTeardownStart,        //  InstanceTeardownStart
    FsMonInstanceTeardownComplete,     //  InstanceTeardownComplete
    FsMonGenerateFileName,             //  GenerateFileName
    FsMonGenerateDestinationFileName,  //  GenerateDestinationFileName
    NULL,                              //  NormalizeNameComponent
#if (NTDDI_VERSION >= NTDDI_VISTA)
    NULL,                              //  Transaction Notification Callback
    FsMonGenerateNormalizeName         //  Filename normalization support callback
#endif // (NTDDI_VERSION >= NTDDI_VISTA)
};

PLIST_ENTRY
GetVolumeMapHeader(void)
{
   return g_pVolumeMapHeader;
}

PFAST_MUTEX
GetVolumeMapMutex(void)
{
   return g_pVolumeMapMutex;
}

NTSTATUS
FsMonInstanceSetup(
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
   _In_ DEVICE_TYPE              VolumeDeviceType,
   _In_ FLT_FILESYSTEM_TYPE      VolumeFilesystemType)
{
   NTSTATUS                            status = STATUS_SUCCESS;
   ULONG                               ulBufferSize = 0;
   PFILTER_VOLUME_STANDARD_INFORMATION pVolumeStandardInfo = NULL;
   PFLT_VOLUME_PROPERTIES              pVolumeProperties = NULL;
   ULONG                               ulRequired = 0;
   PVOLUME_MAP                         pVolumeMap = NULL;

   UNREFERENCED_PARAMETER(Flags);
   UNREFERENCED_PARAMETER(VolumeDeviceType);
   UNREFERENCED_PARAMETER(VolumeFilesystemType);

   PAGED_CODE();

   status = FltGetVolumeInformation(
      pFltObjects->Volume,
      FilterVolumeStandardInformation,
      pVolumeStandardInfo,
      ulBufferSize,
      &ulRequired);

   if (status != STATUS_BUFFER_TOO_SMALL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed calling FltGetVolumeInformation: 0x%X\n",
         status);

      goto Cleanup;
   }

   pVolumeStandardInfo = ExAllocatePool(PagedPool, ulRequired);
   if (pVolumeStandardInfo == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed allocating %u bytes of memory for buffer\n");

      status = STATUS_FLT_DO_NOT_ATTACH;
      goto Cleanup;
   }
   ulBufferSize = ulRequired;
   status = FltGetVolumeInformation(
      pFltObjects->Volume,
      FilterVolumeStandardInformation,
      pVolumeStandardInfo,
      ulBufferSize,
      &ulRequired);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed calling FltGetVolumeInformation: 0x%X\n",
         status);

      goto Cleanup;
   }

   ulBufferSize = 0;
   status = FltGetVolumeProperties(
      pFltObjects->Volume,
      pVolumeProperties,
      ulBufferSize,
      &ulRequired);

   if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed calling FltGetVolumeProperties: 0x%X\n",
         status);

      goto Cleanup;
   }

   pVolumeProperties = ExAllocatePool(NonPagedPool, ulRequired);
   if (pVolumeProperties == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed allocating %u bytes of memory for Volume Properties\n");

      status = STATUS_FLT_DO_NOT_ATTACH;
      goto Cleanup;
   }

   ulBufferSize = ulRequired;
   status = FltGetVolumeProperties(
      pFltObjects->Volume,
      pVolumeProperties,
      ulBufferSize,
      &ulRequired);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed calling FltGetVolumeProperties: 0x%X\n",
         status);

      goto Cleanup;
   }

   pVolumeMap = ExAllocatePool(NonPagedPool, sizeof(VOLUME_MAP));
   if (pVolumeMap == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "FsMonInstanceSetup: Failed allocating %u bytes of memory for Volume Map\n");

      status = STATUS_FLT_DO_NOT_ATTACH;
      goto Cleanup;
   }

   RtlZeroMemory(pVolumeMap, sizeof(VOLUME_MAP));
   RtlCopyMemory(
      pVolumeMap->VolumeName,
      pVolumeStandardInfo->FilterVolumeName,
      pVolumeStandardInfo->FilterVolumeNameLength);

   RtlCopyMemory(
      pVolumeMap->DeviceName,
      pVolumeProperties->FileSystemDeviceName.Buffer,
      pVolumeProperties->FileSystemDeviceName.Length);

   RtlCopyMemory(
      pVolumeMap->DriverName,
      pVolumeProperties->FileSystemDriverName.Buffer,
      pVolumeProperties->FileSystemDriverName.Length);

   if (wcsstr(pVolumeMap->VolumeName, L"\\Device\\HarddiskVolume4")) {
      wcscpy_s(pVolumeMap->MountName, 2, L"C");
   }
   ExAcquireFastMutex(g_pVolumeMapMutex);
   InsertHeadList(g_pVolumeMapHeader, &pVolumeMap->Entry);
   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceSetup: Volume: %ws\n", pVolumeMap->VolumeName);
   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceSetup: Device: %ws\n", pVolumeMap->DeviceName);
   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceSetup: Driver: %ws\n", pVolumeMap->DriverName);
   ExReleaseFastMutex(g_pVolumeMapMutex);

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceSetup\n");

Cleanup:
   if (!NT_SUCCESS(status)) {
      status = STATUS_FLT_DO_NOT_ATTACH;
   }
   if (pVolumeStandardInfo) {
      ExFreePool(pVolumeStandardInfo);
   }
   if (pVolumeProperties) {
      ExFreePool(pVolumeProperties);
   }
   return status;
}


NTSTATUS
FsMonInstanceQueryTeardown(
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(Flags);

   PAGED_CODE();

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceQueryTeardown\n");

   return STATUS_SUCCESS;
}


VOID
FsMonInstanceTeardownStart(
   _In_ PCFLT_RELATED_OBJECTS       pFltObjects,
   _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(Flags);

   PAGED_CODE();

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceTeardownStart\n");
}


VOID
FsMonInstanceTeardownComplete(
   _In_ PCFLT_RELATED_OBJECTS       pFltObjects,
   _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(Flags);

   PAGED_CODE();

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonInstanceTeardownComplete\n");
}

NTSTATUS
DriverEntry(
   _In_ PDRIVER_OBJECT  pDriverObject,
   _In_ PUNICODE_STRING pRegistryPath)
{
   NTSTATUS status;

   status = InitializeSettings(pRegistryPath, &g_pSettings);
   if (!NT_SUCCESS(status)) {
      goto Cleanup;
   }

   g_pVolumeMapHeader = ExAllocatePool(NonPagedPool, sizeof(LIST_ENTRY));
   if (g_pVolumeMapHeader == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "DriverEntry: Failed allocating %u bytes of memory of LIST_ENTRY\n",
         sizeof(LIST_ENTRY));

      goto Cleanup;
   }
   InitializeListHead(g_pVolumeMapHeader);

   g_pVolumeMapMutex = ExAllocatePool(NonPagedPool, sizeof(FAST_MUTEX));
   if (g_pVolumeMapMutex == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "DriverEntry: Failed allocating %u bytes of memory of FAST_MUTEX\n",
         sizeof(FAST_MUTEX));

      goto Cleanup;
   }
   ExInitializeFastMutex(g_pVolumeMapMutex);

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "DriverEntry\n");

   status = FltRegisterFilter(
      pDriverObject,
      &FilterRegistration,
      &g_FilterHandle);

   FLT_ASSERT(NT_SUCCESS(status));

   if (NT_SUCCESS(status)) {
      status = FltStartFiltering(g_FilterHandle);
      if (!NT_SUCCESS(status)) {
         FltUnregisterFilter(g_FilterHandle);
      }
   }

Cleanup:
   return status;
}

NTSTATUS
FsMonUnload(
   _In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
   PLIST_ENTRY    pEntry = NULL;

   UNREFERENCED_PARAMETER(Flags);

   PAGED_CODE();

   DbgPrintEx(
      DPFLTR_DEFAULT_ID,
      0xFFFFFFFF,
      "FsMonUnload\n");

   CleanupSettings(&g_pSettings);

   if (g_pVolumeMapHeader) {
      if (g_pVolumeMapMutex) {
         ExAcquireFastMutex(g_pVolumeMapMutex);
      }

      while (!IsListEmpty(g_pVolumeMapHeader)) {
         pEntry = RemoveTailList(g_pVolumeMapHeader);

         if (pEntry) {
            ExFreePool(pEntry);
         }
      }
      ExFreePool(g_pVolumeMapHeader);

      if (g_pVolumeMapMutex) {
         ExReleaseFastMutex(g_pVolumeMapMutex);
         ExFreePool(g_pVolumeMapMutex);
      }
   }

   FltUnregisterFilter(g_FilterHandle);

   return STATUS_SUCCESS;
}


FLT_PREOP_CALLBACK_STATUS
FsMonGenericPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   LONG                       lPos = -1;
   PFSMON_SETTINGS            pSettings = NULL;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonGenericPreOp: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }

   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "FsMonGenericPreOp failed, status=%08X\n",
            status);
      }
   }
   Cleanup:
   return preOpStatus;
}



VOID
FsMonOpStatusCallback(
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_ PFLT_IO_PARAMETER_BLOCK  pParameterSnapshot,
   _In_ NTSTATUS                 pOpStatus,
   _In_ PVOID                    pRequesterContext)
{
   NTSTATUS          status = STATUS_SUCCESS;
   PFSMON_SETTINGS   pSettings = NULL;
   LONG              lPos = -1;

   UNREFERENCED_PARAMETER(pParameterSnapshot);
   UNREFERENCED_PARAMETER(pOpStatus);
   UNREFERENCED_PARAMETER(pRequesterContext);

   if (pFltObjects && pFltObjects->FileObject && pFltObjects->FileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pFltObjects->FileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonOpStatusCallback: %wZ\n",
               &pFltObjects->FileObject->FileName);
         }
      }
   }
}


FLT_POSTOP_CALLBACK_STATUS
FsMonGenericPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonGenericPostOp: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }

   return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
FsMonPreOpNoPostOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonPreOpNoPostOp: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


BOOLEAN
FsMonDoRequestOpStatus(
   _In_ PFLT_CALLBACK_DATA pData)
{
   PFLT_IO_PARAMETER_BLOCK iopb = pData->Iopb;

   //
   //  return boolean state based on which operations we are interested in
   //

   return (BOOLEAN)

      //
      //  Check for oplock operations
      //

      (((iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL) &&
      ((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK) ||
         (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK) ||
         (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
         (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2)))

         ||

         //
         //    Check for directy change notification
         //

         ((iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL) &&
         (iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY))
         );
}

NTSTATUS
FsMonGenerateFileName(
   _In_     PFLT_INSTANCE           pInstance,
   _In_     PFILE_OBJECT            pFileObject,
   _When_(pFileObject->FsContext != NULL, _In_opt_)
   _When_(pFileObject->FsContext == NULL, _In_)
   PFLT_CALLBACK_DATA               pData,
   _In_     FLT_FILE_NAME_OPTIONS   NameOptions,
   _Out_    PBOOLEAN                pbCacheFileNameInformation,
   _Inout_  PFLT_NAME_CONTROL       pFilename)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;
   BOOLEAN                    bFound = FALSE;
   PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
   PUNICODE_STRING            pUserFileName = NULL;
   WCHAR                      *pCari = NULL;

   UNREFERENCED_PARAMETER(pInstance);
   UNREFERENCED_PARAMETER(pData);
   UNREFERENCED_PARAMETER(NameOptions);
   UNREFERENCED_PARAMETER(pbCacheFileNameInformation);
   UNREFERENCED_PARAMETER(pFilename);

   if (pData) {
      status = UnicodeToFixString(&pData->Iopb->TargetFileObject->FileName, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
      }
      if (pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
         pSettings = GetSettings();
         if (pSettings) {
            status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
            if (!NT_SUCCESS(status) || lPos == -1) {
               status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usRedirectFolder, &lPos, FALSE);
            }
            if (NT_SUCCESS(status) && lPos != -1) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: %wZ\n",
                  &pData->Iopb->TargetFileObject->FileName);

               bFound = TRUE;
            }
         }
      }
   }
   else if (pFileObject) {
      status = UnicodeToFixString(&pFileObject->FileName, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
      }
      if (pFileObject->FileName.Length > 0) {
         pSettings = GetSettings();
         if (pSettings) {
            status = FindString(&pFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
            if (!NT_SUCCESS(status) || lPos == -1) {
               status = FindString(&pFileObject->FileName, &pSettings->usRedirectFolder, &lPos, FALSE);
            }
            if (NT_SUCCESS(status) && lPos != -1) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: pFileObject %wZ\n",
                  &pFileObject->FileName);

               bFound = TRUE;
            }
         }
      }
   }
   if (pFilename) {
      status = UnicodeToFixString(&pFilename->Name, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
      }
   }

   if (bFound) {
      /*
       *  Clear FLT_FILE_NAME_REQUEST_FROM_CURRENT_PROVIDER from the name options
       *  We pass the same name options when we issue a name query to satisfy this
       *  name query. We want that name query to be targeted below simrep.sys and
       *  not recurse into simrep.sys
       */
      ClearFlag(NameOptions, FLT_FILE_NAME_REQUEST_FROM_CURRENT_PROVIDER);

      if (pFileObject->FsContext == NULL) {
         if (pData) {
            status = FltGetFileNameInformation(pData, NameOptions, &pNameInfo);
            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: Failed on FltGetFileNameInformation with callback data %wZ: %X\n",
                  &pData->Iopb->TargetFileObject->FileName,
                  status);

               goto Cleanup;
            }
            pUserFileName = &pNameInfo->Name;
         }
      }
      else {
         if (pData) {
            status = FltGetFileNameInformation(pData, NameOptions, &pNameInfo);
            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: Failed on FltGetFileNameInformation with callback data %wZ: %X\n",
                  &pData->Iopb->TargetFileObject->FileName,
                  status);

               goto Cleanup;
            }
         }
         else {
            status = FltGetFileNameInformationUnsafe(
               pFileObject,
               pInstance,
               NameOptions,
               &pNameInfo);
            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: Failed on FltGetFileNameInformation with FileObject data %wZ: %X\n",
                  &pFileObject->FileName,
                  status);

               goto Cleanup;
            }
         }
         pUserFileName = &pNameInfo->Name;
      }

      status = FltCheckAndGrowNameControl(pFilename, pUserFileName->Length);
      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "FsMonGenerateFileName: Failed on FltCheckAndGrowNameControl %wZ: %X\n",
            pUserFileName,
            status);

         goto Cleanup;
      }
      *pbCacheFileNameInformation = (pFileObject->FsContext != NULL);
   }

Cleanup:
   if (pCari) {
      ExFreePool(pCari);
   }
   if (pNameInfo) {
      FltReleaseFileNameInformation(pNameInfo);
   }
   return status;
}

NTSTATUS
FsMonGenerateDestinationFileName(
   _In_     PFLT_INSTANCE              pInstance,
   _In_     PCUNICODE_STRING           pusParentDirectory,
   _In_     USHORT                     usDeviceNameLength,
   _In_     PCUNICODE_STRING           pusComponent,
   _Out_writes_bytes_(ulExpandComponentNameLength)
   PFILE_NAMES_INFORMATION             pExpandComponentName,
   _In_     ULONG                      ulExpandComponentNameLength,
   _In_     FLT_NORMALIZE_NAME_FLAGS   Flags,
   _Inout_  PVOID                      *pNormalizationCtx)
{
   NTSTATUS             status = STATUS_SUCCESS;
   PFSMON_SETTINGS      pSettings = NULL;
   LONG                 lPos = -1;
   OBJECT_ATTRIBUTES    oa = { 0 };
   BOOLEAN              bIgnoreCase = TRUE;
   HANDLE               hFile = NULL;
   IO_STATUS_BLOCK      iosb = { 0 };
   PFILE_OBJECT         pFileObject = NULL;
   PFLT_CALLBACK_DATA   pData = NULL;
   BOOLEAN              bComponent = TRUE;
   WCHAR                *pCari = NULL;

   UNREFERENCED_PARAMETER(usDeviceNameLength);
   UNREFERENCED_PARAMETER(Flags);
   UNREFERENCED_PARAMETER(pNormalizationCtx);

   PAGED_CODE();

   if (pusComponent) {
      status = UnicodeToFixString((PUNICODE_STRING)pusComponent, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
      }

      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(pusComponent, &pSettings->usTargetFolder, &lPos, FALSE);
         if (!NT_SUCCESS(status) || lPos == -1) {
            status = FindString(pusComponent, &pSettings->usRedirectFolder, &lPos, FALSE);

            if (!NT_SUCCESS(status) || lPos == -1) {
               bComponent = FALSE;
               status = FindString(pusParentDirectory, &pSettings->usTargetFolder, &lPos, FALSE);
               if (!NT_SUCCESS(status) || lPos == -1) {
                  status = FindString(pusParentDirectory, &pSettings->usRedirectFolder, &lPos, FALSE);
               }
            }
            if (NT_SUCCESS(status) && lPos != -1) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: %wZ\n",
                  pusParentDirectory);
            }
         }
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonGenerateFileName: %wZ\n",
               bComponent ? pusComponent : pusParentDirectory);

            if (ulExpandComponentNameLength < sizeof(FILE_NAMES_INFORMATION)) {
               status = STATUS_INVALID_PARAMETER;
               goto Cleanup;
            }

            bIgnoreCase = !BooleanFlagOn(Flags, FLTFL_NORMALIZE_NAME_CASE_SENSITIVE);

            InitializeObjectAttributes(
               &oa,
               (PUNICODE_STRING)pusParentDirectory,
               OBJ_KERNEL_HANDLE | (bIgnoreCase ? OBJ_CASE_INSENSITIVE : 0),
               NULL,
               NULL);

            status = FltCreateFile(
               g_FilterHandle,
               pInstance,
               &hFile,
               FILE_LIST_DIRECTORY | SYNCHRONIZE,
               &oa,
               &iosb,
               NULL,
               FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL,
               FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
               FILE_OPEN,
               FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
               NULL,
               0,
               IO_IGNORE_SHARE_ACCESS_CHECK);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: FltCreateFile() %wZ failed: %X\n",
                  pusParentDirectory,
                  status);

               goto Cleanup;
            }

            status = ObReferenceObjectByHandle(
               hFile,
               FILE_LIST_DIRECTORY | SYNCHRONIZE,
               *IoFileObjectType,
               KernelMode,
               &pFileObject,
               NULL);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: ObReferenceObjectByHandle() %wZ failed: %X\n",
                  pusParentDirectory,
                  status);

               goto Cleanup;
            }

            status = FltQueryDirectoryFile(
               pInstance,
               pFileObject,
               pExpandComponentName,
               ulExpandComponentNameLength,
               FileNamesInformation,
               TRUE,
               (PUNICODE_STRING)pusComponent,
               TRUE,
               NULL);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateFileName: FltQueryDirectoryFile() %wZ failed: %X\n",
                  pusParentDirectory,
                  status);

               status = FltAllocateCallbackData(
                  pInstance,
                  pFileObject,
                  &pData);

               if (!NT_SUCCESS(status)) {
                  DbgPrintEx(
                     DPFLTR_DEFAULT_ID,
                     0xFFFFFFFF,
                     "FsMonGenerateFileName: FltAllocateCallbackData() %wZ failed: %X\n",
                     pusParentDirectory,
                     status);

                  goto Cleanup;
               }

               pData->Iopb->MajorFunction = IRP_MJ_DIRECTORY_CONTROL;
               pData->Iopb->MinorFunction = IRP_MN_QUERY_DIRECTORY;

               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length = ulExpandComponentNameLength;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName = (PUNICODE_STRING)pusComponent;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass = FileNamesInformation;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileIndex = 0;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer = pExpandComponentName;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress = NULL;

               pData->Iopb->OperationFlags = pData->Iopb->OperationFlags | SL_RESTART_SCAN | SL_RETURN_SINGLE_ENTRY;

               FltPerformSynchronousIo(pData);

               status = pData->IoStatus.Status;
               FltFreeCallbackData(pData);
               pData = NULL;
            }
         }
      }
   }
   else if (pusParentDirectory) {
      if (pCari) {
         ExFreePool(pCari);
         pCari = NULL;
      }
      status = UnicodeToFixString((PUNICODE_STRING)pusParentDirectory, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
      }
   }

Cleanup:
   if (pCari) {
      ExFreePool(pCari);
   }
   if (pFileObject) {
      ObDereferenceObject(pFileObject);
   }
   if (hFile) {
      FltClose(hFile);
   }
   return status;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
NTSTATUS
FsMonGenerateNormalizeName(
   _In_ PFLT_INSTANCE               pInstance,
   _In_ PFILE_OBJECT                pFileObject,
   _In_ PCUNICODE_STRING            pusParentDirectory,
   _In_ USHORT                      usDeviceNameLength,
   _In_ PCUNICODE_STRING            pusComponent,
   _Out_writes_bytes_(pExpandComponentNameLength) PFILE_NAMES_INFORMATION pExpandComponentName,
   _In_ ULONG                       ulExpandComponentNameLength,
   _In_ FLT_NORMALIZE_NAME_FLAGS    Flags,
   _Inout_ PVOID                    *pNormalizationCtx)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;
   HANDLE                     hDir = NULL;
   PFILE_OBJECT               pDirObject = NULL;
   OBJECT_ATTRIBUTES          oa = { 0 };
   IO_STATUS_BLOCK            iosb = { 0 };
   BOOLEAN                    bIgnoreCase = TRUE;
   IO_DRIVER_CREATE_CONTEXT   IoCreateCtx = { 0 };
   TXN_PARAMETER_BLOCK        TxnBlock = { 0 };
   PTXN_PARAMETER_BLOCK       pTxnOriginalBlock = NULL;
   PFLT_CALLBACK_DATA         pData = NULL;
   WCHAR                      *pCari = NULL;

   UNREFERENCED_PARAMETER(pNormalizationCtx);
   UNREFERENCED_PARAMETER(usDeviceNameLength);

   PAGED_CODE();

   if (pusComponent) {
      status = UnicodeToFixString((PUNICODE_STRING)pusComponent, &pCari);
      if (NT_SUCCESS(status)) {
         if (wcsstr(pCari, L"Coba")) {
            __debugbreak();
         }
         else if (wcsstr(pCari, L"Alih")) {
            __debugbreak();
         }
         else {
            if (pFileObject) {
               if (pCari) {
                  ExFreePool(pCari);
                  pCari = NULL;
               }
               status = UnicodeToFixString(&pFileObject->FileName, &pCari);
               if (NT_SUCCESS(status)) {
                  if (wcsstr(pCari, L"Coba")) {
                     __debugbreak();
                  }
                  else if (wcsstr(pCari, L"Alih")) {
                     __debugbreak();
                  }
               }
            }
         }
      }
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(pusComponent, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "FsMonGenerateNormalizeName: %wZ\n",
               pusParentDirectory);

            if (ulExpandComponentNameLength < sizeof(FILE_NAMES_INFORMATION)) {
               status = STATUS_INVALID_PARAMETER;
               goto Cleanup;
            }

            bIgnoreCase = !BooleanFlagOn(Flags, FLTFL_NORMALIZE_NAME_CASE_SENSITIVE);

            InitializeObjectAttributes(
               &oa,
               (PUNICODE_STRING)pusParentDirectory,
               OBJ_KERNEL_HANDLE | (bIgnoreCase ? OBJ_CASE_INSENSITIVE : 0),
               NULL,
               NULL);

            /*
             *  On Vista and beyond, we need to query the normalized name in the context
             *  of the same transaction as the name query
             */

            IoInitializeDriverCreateContext(&IoCreateCtx);

            pTxnOriginalBlock = IoGetTransactionParameterBlock(pFileObject);
            if (pTxnOriginalBlock) {
               TxnBlock.Length = sizeof(TxnBlock);
               TxnBlock.TransactionObject = pTxnOriginalBlock->TransactionObject;
               TxnBlock.TxFsContext = TXF_MINIVERSION_DEFAULT_VIEW;

               IoCreateCtx.TxnParameters = &TxnBlock;
            }

            status = FltCreateFileEx2(
               g_FilterHandle,
               pInstance,
               &hDir,
               &pDirObject,
               FILE_LIST_DIRECTORY | SYNCHRONIZE,
               &oa,
               &iosb,
               NULL,
               FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL,
               FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
               FILE_OPEN,
               FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
               NULL,
               0,
               IO_IGNORE_SHARE_ACCESS_CHECK,
               &IoCreateCtx);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateNormalizeName: FltCreateFileEx2() %wZ failed: %X\n",
                  pusParentDirectory,
                  status);

               goto Cleanup;
            }

            status = FltQueryDirectoryFile(
               pInstance,
               pDirObject,
               pExpandComponentName,
               ulExpandComponentNameLength,
               FileNamesInformation,
               TRUE,
               (PUNICODE_STRING)pusComponent,
               TRUE,
               NULL);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "FsMonGenerateNormalizeName: FltQueryDirectoryFile() %wZ failed: %X\n",
                  pusParentDirectory,
                  status);

               status = FltAllocateCallbackData(
                  pInstance,
                  pFileObject,
                  &pData);

               if (!NT_SUCCESS(status)) {
                  DbgPrintEx(
                     DPFLTR_DEFAULT_ID,
                     0xFFFFFFFF,
                     "FsMonGenerateFileName: FltAllocateCallbackData() %wZ failed: %X\n",
                     pusParentDirectory,
                     status);

                  goto Cleanup;
               }

               pData->Iopb->MajorFunction = IRP_MJ_DIRECTORY_CONTROL;
               pData->Iopb->MinorFunction = IRP_MN_QUERY_DIRECTORY;

               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.Length = ulExpandComponentNameLength;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName = (PUNICODE_STRING)pusComponent;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass = FileNamesInformation;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.FileIndex = 0;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer = pExpandComponentName;
               pData->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress = NULL;

               pData->Iopb->OperationFlags = pData->Iopb->OperationFlags | SL_RESTART_SCAN | SL_RETURN_SINGLE_ENTRY;

               FltPerformSynchronousIo(pData);

               status = pData->IoStatus.Status;
               FltFreeCallbackData(pData);
               pData = NULL;
            }
         }
      }
   }

Cleanup:
   if (pCari) {
      ExFreePool(pCari);
   }
   if (pDirObject) {
      ObDereferenceObject(pDirObject);
   }
   if (hDir) {
      FltClose(hDir);
   }
   return status;
}
#endif

FLT_PREOP_CALLBACK_STATUS
FsMonCreatePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   UNICODE_STRING             usNormalizeName = { 0 };
   BOOLEAN                    bOpenDirectory = FALSE;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   PAGED_CODE();

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (FlagOn(pData->Iopb->OperationFlags, SL_OPEN_PAGING_FILE)) {
      preOpStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
      goto Cleanup;
   }

   if (FlagOn(pData->Iopb->TargetFileObject->Flags, FO_VOLUME_OPEN)) {
      preOpStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
      goto Cleanup;
   }

   if (FlagOn(pData->Iopb->Parameters.Create.Options, FILE_OPEN_BY_FILE_ID)) {
      preOpStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
      goto Cleanup;
   }

   if (NeedRedirect(pData, &usNormalizeName)) {
      if (bOpenDirectory) {
         SetFlag(pData->Iopb->OperationFlags, SL_OPEN_TARGET_DIRECTORY);
      }
      preOpStatus = RedirectFile(pData, &usNormalizeName);

      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_CREATE Pre: Redirected to %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);
   }

   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_CREATE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   FreeUnicodeString(&usNormalizeName);

   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonCreatePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CREATE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonReadPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_READ Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_READ Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonReadPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_READ Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_WRITE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_WRITEPre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_WRITE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonCreateNamedPipePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CREATE_NAMED_PIPE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_CREATE_NAMED_PIPE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonCreateNamedPipePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CREATE_NAMED_PIPE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonClosePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   UNICODE_STRING             usNormalizeName = { 0 };
   BOOLEAN                    bOpenDirectory = FALSE;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_CLOSE Pre: Fast IO disallowed %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);
      goto Cleanup;
   }
   if (NeedRedirect(pData, &usNormalizeName)) {
      if (bOpenDirectory) {
         SetFlag(pData->Iopb->OperationFlags, SL_OPEN_TARGET_DIRECTORY);
      }
      preOpStatus = RedirectFile(pData, &usNormalizeName);

      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_CLOSE Pre: Redirected to %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);
   }


   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_CLOSE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   FreeUnicodeString(&usNormalizeName);

   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonClosePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CLOSE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonQueryInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_INFORMATION Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_QUERY_INFORMATION Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_INFORMATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonSetInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   UNICODE_STRING             usNormalizeName = { 0 };
   FILE_INFORMATION_CLASS     FileInfoClass;
   PFILE_RENAME_INFORMATION   pRenameInfo = NULL;
   PFILE_RENAME_INFORMATION   pNewRenameInfo = NULL;
   PFILE_LINK_INFORMATION     pLinkInfo = NULL;
   PFSMON_SETTINGS            pSettings = NULL;
   PVOID                      pNew = NULL;
   WCHAR                      *pNormalized = NULL;
   WCHAR                      *pNewFileName = NULL;
   SIZE_T                     length = 0;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }
   pSettings = GetSettings();
   if (!pSettings) {
      goto Cleanup;
   }

   FileInfoClass = pData->Iopb->Parameters.SetFileInformation.FileInformationClass;

   switch (FileInfoClass) {
   case FileRenameInformation:
   case FileRenameInformationEx:
      pRenameInfo = pData->Iopb->Parameters.SetFileInformation.InfoBuffer;
      break;

   case FileLinkInformation:
      pLinkInfo = pData->Iopb->Parameters.SetFileInformation.InfoBuffer;
      break;

   default:
      goto Cleanup;
   }
   if (NeedRedirect(pData, &usNormalizeName)) {
      preOpStatus = RedirectFile(pData, &usNormalizeName);

      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_SET_INFORMATION Pre: Redirected to %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);
   }

   if (pRenameInfo) {
      status = NormalizeFileName(pRenameInfo->FileName, pRenameInfo->FileNameLength, &pNormalized);
      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_INFORMATION Pre: Failed normalizing filename of %ws\n",
            pRenameInfo->FileName);

         goto Cleanup;
      }

      InitUnicodeFromString(PagedPool, &usNormalizeName, pNormalized, FSMON_TAG);

      status = FindString(&usNormalizeName, &pSettings->usTargetFolder, &lPos, FALSE);
      if (lPos != -1) {
         status = ReplaceUnicodeString(
            PagedPool,
            &usNormalizeName,
            &pSettings->usTargetFolder,
            &pSettings->usRedirectFolder);

         if (!NT_SUCCESS(status)) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_INFORMATION Pre: Failed replacing filename %ws: 0x%X\n",
               pNormalized,
               status);

            goto Cleanup;
         }

         UnicodeToFixString(&usNormalizeName, &pNewFileName);
         length = wcslen(pNewFileName) * sizeof(WCHAR);

         length = FIELD_OFFSET(FILE_RENAME_INFORMATION, FileName) + length;

         pNew = ExAllocatePool(PagedPool, length);
         if (pNew == NULL) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_INFORMATION Pre: Failed allocating %u bytes of memory\n",
               length * sizeof(WCHAR));

            goto Cleanup;
         }

         length = wcslen(pNewFileName) * sizeof(WCHAR);

         pNewRenameInfo = (PFILE_RENAME_INFORMATION)pNew;
         pNewRenameInfo->Flags = pRenameInfo->Flags;
         pNewRenameInfo->RootDirectory = NULL;
         pNewRenameInfo->ReplaceIfExists = pRenameInfo->ReplaceIfExists;
         pNewRenameInfo->FileNameLength = (ULONG)length;

         RtlCopyMemory(pNewRenameInfo->FileName, pNewFileName, length);

         status = FltSetInformationFile(
            pFltObjects->Instance,
            pFltObjects->FileObject,
            pNew,
            (ULONG)length,
            FileInfoClass);

         pData->IoStatus.Status = status;
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_INFORMATION Pre: Changed destination %ws -> %ws -> %ws\n",
            pRenameInfo->FileName,
            pNormalized,
            pNewFileName);

         //__debugbreak();
      }
   }

   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_INFORMATION Pre failed, status=%08X\n",
            status);
      }
   }
   preOpStatus = FLT_PREOP_COMPLETE;

Cleanup:
   if (pNew) {
      ExFreePool(pNew);
   }
   if (pNewFileName) {
      ExFreePool(pNewFileName);
   }
   if (pNormalized) {
      ExFreePool(pNormalized);
   }
   FreeUnicodeString(&usNormalizeName);

   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonSetInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_INFORMATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonQueryEaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_EA Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_QUERY_EA Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryEaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_EA Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonSetEaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_EA Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_EA Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonSetEaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_EA Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonFlushBuffersPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FLUSH_BUFFERS Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_FLUSH_BUFFERS Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;

}

FLT_POSTOP_CALLBACK_STATUS
FsMonFlushBuffersPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FLUSH_BUFFERS Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonQueryVolumeInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_VOLUME_INFORMATION Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_QUERY_VOLUME_INFORMATION Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryVolumeInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_VOLUME_INFORMATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonSetVolumeInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_VOLUME_INFORMATION Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_VOLUME_INFORMATION Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonSetVolumeInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_VOLUME_INFORMATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonDirCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   UNICODE_STRING             usNormalizeName = { 0 };

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   PAGED_CODE();

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_DIRECTORY_CONTROL Pre: Fast IO not allowed %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);

      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (NeedRedirect(pData, &usNormalizeName)) {
      preOpStatus = RedirectFile(pData, &usNormalizeName);

      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "IRP_MJ_DIRECTORY_CONTROL Pre: Redirected to %wZ\n",
         &pData->Iopb->TargetFileObject->FileName);
   }

   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_DIRECTORY_CONTROL Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   FreeUnicodeString(&usNormalizeName);
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonDirCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_DIRECTORY_CONTROL Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonFsCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FILE_SYSTEM_CONTROL Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_FILE_SYSTEM_CONTROL Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonFsCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FILE_SYSTEM_CONTROL Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonDevCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_DEVICE_CONTROL Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_DEVICE_CONTROL Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonDevCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_DEVICE_CONTROL Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonIntDevCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_INTERNAL_DEVICE_CONTROL Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_INTERNAL_DEVICE_CONTROL Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonIntDevCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_INTERNAL_DEVICE_CONTROL Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonShutdownPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SHUTDOWN Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SHUTDOWN Pre failed, status=%08X\n",
            status);
      }
   }
   return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS
FsMonLockCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_LOCK_CONTROL Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_LOCK_CONTROL Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonLockCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_LOCK_CONTROL Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonCleanupPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SYNCHRONIZE;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CLEANUP Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_CLEANUP Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonCleanupPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CLEANUP Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonCreateMailSlotPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CREATE_MAILSLOT Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_CREATE_MAILSLOT Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonCreateMailSlotPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_CREATE_MAILSLOT Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonQuerySecurityPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_SECURITY Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_QUERY_SECURITY Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonQuerySecurityPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_SECURITY Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonSetSecurityPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_SECURITY Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_SECURITY Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonSetSecurityPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_SECURITY Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonQueryQuotaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_QUOTA Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_QUERY_QUOTA Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryQuotaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_QUERY_QUOTA Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonSetQuotaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_QUOTA Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_SET_QUOTA Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonSetQuotaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_SET_QUOTA Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonPnpPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_PNP Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_PNP Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonPnpPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_PNP Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireSectionSyncPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireSectionSyncPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseSectionSyncPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseSectionSyncPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireModWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_MOD_WRITE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_ACQUIRE_FOR_MOD_WRITE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireModWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_MOD_WRITE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseModWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_MOD_WRITE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_RELEASE_FOR_MOD_WRITE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseModWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_MOD_WRITE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireCcFlushPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_CC_FLUSH Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_ACQUIRE_FOR_CC_FLUSH Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireCcFlushPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_ACQUIRE_FOR_CC_FLUSH Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseCcFlushPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_CC_FLUSH Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_RELEASE_FOR_CC_FLUSH Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseCcFlushPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_RELEASE_FOR_CC_FLUSH Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonFastIoCheckPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE Pre failed, status=%08X\n",
            status);
      }
   }
   return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonFastIoCheckPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonNetQueryOpenPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
   PIO_STACK_LOCATION         pIrpStackLocation = NULL;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   FLT_ASSERT(pData->Iopb->MajorFunction == IRP_MJ_NETWORK_QUERY_OPEN);
   FLT_ASSERT(FLT_IS_FASTIO_OPERATION(pData));

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_NETWORK_QUERY_OPEN Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);

            if (FLT_IS_FASTIO_OPERATION(pData)) {
               preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Fast IO not allowed %wZ\n",
                  &pData->Iopb->TargetFileObject->FileName);
               goto Cleanup;
            }

            pIrpStackLocation = IoGetCurrentIrpStackLocation(pData->Iopb->Parameters.NetworkQueryOpen.Irp);
            /*
             *  Check if this is a paging file as we don't want to redirect
             *  the location of the paging file.
             */
            if (FlagOn(pIrpStackLocation->Flags, SL_OPEN_PAGING_FILE)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Not opening paging %wZ\n",
                  &pData->Iopb->TargetFileObject->FileName);

               goto Cleanup;
            }
            /*
             *  We are not allowing volume opens to be reparsed in the sample.
             */
            if (FlagOn(pData->Iopb->TargetFileObject->Flags, FO_VOLUME_OPEN)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Ignoring volume open %wZ\n",
                  &pData->Iopb->TargetFileObject->FileName);

               goto Cleanup;
            }

            if (FlagOn(pIrpStackLocation->Parameters.Create.Options, FILE_OPEN_BY_FILE_ID)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Ignoring open by file ID %wZ\n",
                  &pData->Iopb->TargetFileObject->FileName);

               goto Cleanup;
            }
            /*
             *  A rename should never come on the fast IO path
             */
            FLT_ASSERT(pIrpStackLocation->Flags != SL_OPEN_TARGET_DIRECTORY);

            status = FltGetFileNameInformation(
               pData,
               FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_DEFAULT,
               &pNameInfo);

            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Failed FltGetFileNameInformation() %wZ: %X\n",
                  &pData->Iopb->TargetFileObject->FileName,
                  status);

               goto Cleanup;
            }

            status = FltParseFileNameInformation(pNameInfo);
            if (!NT_SUCCESS(status)) {
               DbgPrintEx(
                  DPFLTR_DEFAULT_ID,
                  0xFFFFFFFF,
                  "IRP_MJ_NETWORK_QUERY_OPEN Pre: Failed FltParseFileNameInformation() %wZ: %X\n",
                  &pData->Iopb->TargetFileObject->FileName,
                  status);

               goto Cleanup;
            }
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_NETWORK_QUERY_OPEN Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   if (pNameInfo) {
      FltReleaseFileNameInformation(pNameInfo);
   }
   if (!NT_SUCCESS(status)) {
      preOpStatus = FLT_PREOP_COMPLETE;
      pData->IoStatus.Status = status;
   }
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonNetQueryOpenPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_NETWORK_QUERY_OPEN Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonMdlReadPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_READ Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_MDL_READ Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlReadPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_READ Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonMdlReadCompletePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_READ_COMPLETE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_MDL_READ_COMPLETE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlReadCompletePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_READ_COMPLETE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonPrepareMdlWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_PREPARE_MDL_WRITE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_PREPARE_MDL_WRITE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonPrepareMdlWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_PREPARE_MDL_WRITE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonMdlWriteCompletePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_WRITE_COMPLETE Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_MDL_WRITE_COMPLETE Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlWriteCompletePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_MDL_WRITE_COMPLETE Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonVolumeMountPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_VOLUME_MOUNT Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_VOLUME_MOUNT Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonVolumeMountPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_VOLUME_MOUNT Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsMonVolumeDismountPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);

   if (FLT_IS_FASTIO_OPERATION(pData)) {
      preOpStatus = FLT_PREOP_DISALLOW_FASTIO;
      goto Cleanup;
   }

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_VOLUME_DISMOUNT Pre: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   if (FsMonDoRequestOpStatus(pData)) {
      status = FltRequestOperationStatusCallback(
         pData,
         FsMonOpStatusCallback,
         (PVOID)(++g_ulOperationStatusCtx));

      if (!NT_SUCCESS(status)) {
         DbgPrintEx(
            DPFLTR_DEFAULT_ID,
            0xFFFFFFFF,
            "IRP_MJ_VOLUME_DISMOUNT Pre failed, status=%08X\n",
            status);
      }
   }
Cleanup:
   return preOpStatus;
}

FLT_POSTOP_CALLBACK_STATUS
FsMonVolumeDismountPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   UNREFERENCED_PARAMETER(pFltObjects);
   UNREFERENCED_PARAMETER(pCompletionContext);
   UNREFERENCED_PARAMETER(Flags);

   if (pData && pData->Iopb->TargetFileObject && pData->Iopb->TargetFileObject->FileName.Length > 0) {
      pSettings = GetSettings();
      if (pSettings) {
         status = FindString(&pData->Iopb->TargetFileObject->FileName, &pSettings->usTargetFolder, &lPos, FALSE);
         if (NT_SUCCESS(status) && lPos != -1) {
            DbgPrintEx(
               DPFLTR_DEFAULT_ID,
               0xFFFFFFFF,
               "IRP_MJ_VOLUME_DISMOUNT Post: %wZ\n",
               &pData->Iopb->TargetFileObject->FileName);
         }
      }
   }
   return FLT_POSTOP_FINISHED_PROCESSING;
}

PFSMON_SETTINGS
GetSettings(
   void)
{
   return g_pSettings;
}