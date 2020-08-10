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

    File name: FsMon.h
    Contact: hdmih@yahoo.com
    Created: 04-07-2020
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#pragma once

typedef struct _VOLUME_MAP {
   LIST_ENTRY  Entry;
   WCHAR       VolumeName[1024];
   WCHAR       DeviceName[1024];
   WCHAR       DriverName[1024];
   WCHAR       MountName[2];
} VOLUME_MAP, *PVOLUME_MAP;

EXTERN_C_START

PLIST_ENTRY
GetVolumeMapHeader(void);

PFAST_MUTEX
GetVolumeMapMutex(void);

NTSTATUS
DriverEntry(
   _In_ PDRIVER_OBJECT  pDriverObject,
   _In_ PUNICODE_STRING pRegistryPath);

NTSTATUS
FsMonInstanceSetup(
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
   _In_ DEVICE_TYPE              VolumeDeviceType,
   _In_ FLT_FILESYSTEM_TYPE      VolumeFilesystemType);

VOID
FsMonInstanceTeardownStart(
   _In_ PCFLT_RELATED_OBJECTS       pFltObjects,
   _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags);

VOID
FsMonInstanceTeardownComplete(
   _In_ PCFLT_RELATED_OBJECTS       pFltObjects,
   _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags);

NTSTATUS
FsMonUnload(
   _In_ FLT_FILTER_UNLOAD_FLAGS Flags);

NTSTATUS
FsMonInstanceQueryTeardown(
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonGenericPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

VOID
FsMonOpStatusCallback(
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_ PFLT_IO_PARAMETER_BLOCK  pParameterSnapshot,
   _In_ NTSTATUS                 pOpStatus,
   _In_ PVOID                    pRequesterContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonGenericPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonPreOpNoPostOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

BOOLEAN
FsMonDoRequestOpStatus(
   _In_ PFLT_CALLBACK_DATA pData);

NTSTATUS
FsMonGenerateFileName(
   _In_     PFLT_INSTANCE           pInstance,
   _In_     PFILE_OBJECT            pFileObject,
   _When_(pFileObject->FsContext != NULL, _In_opt_)
   _When_(pFileObject->FsContext == NULL, _In_)
   PFLT_CALLBACK_DATA               pData,
   _In_     FLT_FILE_NAME_OPTIONS   NameOptions,
   _Out_    PBOOLEAN                pbCacheFileNameInformation,
   _Inout_  PFLT_NAME_CONTROL       pFilename);

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
   _Inout_  PVOID                      *pNormalizationCtx);

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
   _Inout_ PVOID                    *pNormalizationCtx);
#endif

FLT_PREOP_CALLBACK_STATUS
FsMonCreatePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonCreatePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonReadPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonReadPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonCreateNamedPipePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonCreateNamedPipePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonClosePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonClosePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonQueryInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonSetInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonSetInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonQueryEaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryEaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonSetEaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonSetEaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonFlushBuffersPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonFlushBuffersPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonQueryVolumeInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryVolumeInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonSetVolumeInfoPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonSetVolumeInfoPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonDirCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonDirCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonFsCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonFsCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonDevCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonDevCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonIntDevCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonIntDevCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonShutdownPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_PREOP_CALLBACK_STATUS
FsMonLockCtrlPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonLockCtrlPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonCleanupPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonCleanupPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonCreateMailSlotPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonCreateMailSlotPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonQuerySecurityPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonQuerySecurityPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonSetSecurityPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonSetSecurityPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonQueryQuotaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonQueryQuotaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonSetQuotaPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonSetQuotaPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonPnpPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonPnpPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireSectionSyncPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireSectionSyncPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseSectionSyncPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseSectionSyncPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireModWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireModWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseModWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseModWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonAcquireCcFlushPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonAcquireCcFlushPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonReleaseCcFlushPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonReleaseCcFlushPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonFastIoCheckPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonFastIoCheckPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonNetQueryOpenPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonNetQueryOpenPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonMdlReadPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlReadPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonMdlReadCompletePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlReadCompletePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonPrepareMdlWritePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonPrepareMdlWritePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonMdlWriteCompletePreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonMdlWriteCompletePostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonVolumeMountPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonVolumeMountPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
FsMonVolumeDismountPreOp(
   _Inout_ PFLT_CALLBACK_DATA             pData,
   _In_ PCFLT_RELATED_OBJECTS             pFltObjects,
   _Flt_CompletionContext_Outptr_ PVOID   *pCompletionContext);

FLT_POSTOP_CALLBACK_STATUS
FsMonVolumeDismountPostOp(
   _Inout_ PFLT_CALLBACK_DATA    pData,
   _In_ PCFLT_RELATED_OBJECTS    pFltObjects,
   _In_opt_ PVOID                pCompletionContext,
   _In_ FLT_POST_OPERATION_FLAGS Flags);

PFSMON_SETTINGS
GetSettings(
   void);

EXTERN_C_END
