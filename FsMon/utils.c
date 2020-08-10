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

    File name: utils.c
    Contact: hdmih@yahoo.com
    Created: 04-07-2020
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <fltKernel.h>
#include <dontuse.h>

#include "struct.h"
#include "FsMon.h"
#include "utils.h"

NTSTATUS
FindString(
   _In_  PCUNICODE_STRING  pusString,
   _In_  PCUNICODE_STRING  pusSearch,
   _In_  LONG              *plPos,
   _In_  BOOLEAN           bCaseSensitive)
{
   NTSTATUS       status = STATUS_SUCCESS;
   UNICODE_STRING usSearch = { 0 };
   UNICODE_STRING usString = { 0 };
   LONG           lSearchLength = 0;
   LONG           lIdx1 = 0;
   LONG           lIdx2 = 0;
   POOL_TYPE      poolType = PagedPool;
   KIRQL          irql = PASSIVE_LEVEL;
   WCHAR          *pString = NULL;
   WCHAR          *pSearch = NULL;

   *plPos = -1;

   if (pusString->Length == 0) {
      goto Cleanup;
   }

   irql = KeGetCurrentIrql();
   if (irql > APC_LEVEL) {
      goto Cleanup;
   }
   if (irql != PASSIVE_LEVEL) {
      poolType = NonPagedPoolNx;
   }

   /*
    *  According to MSDN we can provide the memory for the string with giving
    *  FALSE for 3rd arguments. But looks like this will not be clearly removing
    *  the memory, so we use to give TRUE.
    */
   if (bCaseSensitive == FALSE) {
      status = RtlUpcaseUnicodeString(
         &usSearch,
         pusSearch,
         TRUE);

      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }

      status = RtlUpcaseUnicodeString(
         &usString,
         pusString,
         TRUE);

      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }
   }
   else {
      status = CopyUnicodeString(poolType, &usSearch, pusSearch, FSMON_TAG);
      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }

      status = CopyUnicodeString(poolType, &usString, pusString, FSMON_TAG);
      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }
   }
   UnicodeToFixString(&usString, &pString);
   UnicodeToFixString(&usSearch, &pSearch);
   if (wcsstr(pString, pSearch)) {
      //__debugbreak();
   }
   lSearchLength = (LONG)(usSearch.Length / (USHORT)sizeof(WCHAR));
   for (lIdx1 = 0;
      lIdx1 <= ((LONG)(usString.Length / (USHORT)sizeof(WCHAR)) - lSearchLength);
      lIdx1++) {

      for (lIdx2 = 0; lIdx2 < lSearchLength; lIdx2++) {
         if (usString.Buffer[lIdx1 + lIdx2] != usSearch.Buffer[lIdx2]) {
            break;
         }
      }
      if (lSearchLength == lIdx2) {
         *plPos = lIdx1;
         break;
      }
   }

Cleanup:
   if (pString) {
      ExFreePool(pString);
   }
   if (pSearch) {
      ExFreePool(pSearch);
   }
   if (bCaseSensitive == FALSE) {
      if (usSearch.Buffer) {
         RtlFreeUnicodeString(&usSearch);
      }
      if (usString.Buffer) {
         RtlFreeUnicodeString(&usString);
      }
   }
   else {
      FreeUnicodeString(&usSearch);
      FreeUnicodeString(&usString);
   }

   return status;
}

VOID
FreeUnicodeString(
   _Inout_  PUNICODE_STRING   pString)
{
   NT_ASSERT(pString);

   if (pString->Buffer != NULL) {
      ExFreePool(pString->Buffer);

      pString->Buffer = NULL;
      pString->Length = pString->MaximumLength = 0;
   }
}

NTSTATUS
CopyUnicodeString(
   _In_     POOL_TYPE         poolType,
   _Inout_  PUNICODE_STRING   pDest,
   _In_     PCUNICODE_STRING  pSrc,
   _In_     ULONG             tag)
{
   NTSTATUS          status = STATUS_SUCCESS;

   if (pSrc == NULL || pDest == NULL) {
      status = STATUS_INVALID_PARAMETER;
      goto Cleanup;
   }

   FreeUnicodeString(pDest);

   if (tag == 0) {
      pDest->Buffer = ExAllocatePool(
         poolType,
         pSrc->MaximumLength + (USHORT)sizeof(WCHAR));
   }
   else {
      pDest->Buffer = ExAllocatePoolWithTag(
         poolType,
         pSrc->MaximumLength + (USHORT)sizeof(WCHAR),
         tag);
   }

   if (pDest->Buffer == NULL) {
      status = STATUS_INSUFFICIENT_RESOURCES;
      goto Cleanup;
   }
   RtlZeroMemory(pDest->Buffer, pSrc->MaximumLength + (USHORT)sizeof(WCHAR));
   RtlCopyMemory(pDest->Buffer, pSrc->Buffer, pSrc->MaximumLength);
   pDest->Length = pSrc->Length;
   pDest->MaximumLength = pSrc->MaximumLength;

Cleanup:
   return status;
}

BOOLEAN
EqualUnicodeString(
   _In_  PUNICODE_STRING   pusString1,
   _In_  PUNICODE_STRING   pusString2,
   _In_  BOOLEAN           bCaseSensitive)
{
   BOOLEAN  bRet = FALSE;
   WCHAR    *pPtr1 = NULL;
   WCHAR    *pPtr2 = NULL;
   USHORT   usIdx = 0;
   USHORT   usLength1 = 0;
   USHORT   usLength2 = 0;

   PAGED_CODE();

   NT_ASSERT(pusString1);
   NT_ASSERT(pusString2);

   if (pusString1->Length != pusString2->Length) {
      goto Cleanup;
   }

   if (bCaseSensitive) {
      usLength1 = pusString1->Length / sizeof(WCHAR);
      if (pusString1->Buffer[usLength1 - 1] == L'\0') {
         usLength1--;
      }

      usLength2 = pusString2->Length / sizeof(WCHAR);
      if (pusString2->Buffer[usLength2 - 1] == L'\0') {
         usLength2--;
      }

      if (usLength1 != usLength2) {
         goto Cleanup;
      }
      pPtr1 = pusString1->Buffer;
      pPtr2 = pusString2->Buffer;

      for (usIdx = 0; usIdx < usLength1; usIdx++) {
         if (pPtr1[usIdx] != pPtr2[usIdx]) {
            break;
         }
      }
      if (usIdx == usLength1) {
         bRet = TRUE;
      }
   }
   else {
      usLength1 = pusString1->Length / sizeof(WCHAR);
      if (pusString1->Buffer[usLength1 - 1] == L'\0') {
         usLength1--;
      }
      usLength1++;

      pPtr1 = ExAllocatePoolWithTag(
         PagedPool,
         usLength1 * sizeof(WCHAR),
         FSMON_TAG);

      if (pPtr1 == NULL) {
         goto Cleanup;
      }
      RtlZeroMemory(pPtr1, usLength1 * sizeof(WCHAR));
      RtlCopyMemory(pPtr1, pusString1->Buffer, pusString1->Length);
      _wcsupr(pPtr1);

      usLength2 = pusString2->Length / sizeof(WCHAR);
      if (pusString2->Buffer[usLength2 - 1] == L'\0') {
         usLength2--;
      }
      usLength2++;

      pPtr2 = ExAllocatePoolWithTag(
         PagedPool,
         usLength2 * sizeof(WCHAR),
         FSMON_TAG);

      if (pPtr2 == NULL) {
         goto Cleanup;
      }
      RtlZeroMemory(pPtr2, usLength2 * sizeof(WCHAR));
      RtlCopyMemory(pPtr2, pusString2->Buffer, pusString2->Length);
      _wcsupr(pPtr2);

      usLength1 = (USHORT)wcslen(pPtr1);
      usLength2 = (USHORT)wcslen(pPtr2);

      if (usLength1 != usLength2) {
         goto Cleanup;
      }

      for (usIdx = 0; usIdx < usLength1; usIdx++) {
         if (pPtr1[usIdx] != pPtr2[usIdx]) {
            break;
         }
      }
      if (usIdx == usLength1) {
         bRet = TRUE;
      }
   }

Cleanup:
   if (pPtr1) {
      ExFreePool(pPtr1);
      pPtr1 = NULL;
   }
   if (pPtr2) {
      ExFreePool(pPtr2);
      pPtr2 = NULL;
   }
   return bRet;
}

FLT_PREOP_CALLBACK_STATUS
RedirectFile(
   _In_  PFLT_CALLBACK_DATA   pData,
   _In_  PCUNICODE_STRING     pNormalizedName)
{
   NTSTATUS                   status = STATUS_SUCCESS;
   FLT_PREOP_CALLBACK_STATUS  preOpStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
   PFSMON_SETTINGS            pSettings = NULL;
   UNICODE_STRING             usFileName = { 0 };
   WCHAR                      *pFileName = NULL;

   UNREFERENCED_PARAMETER(pNormalizedName);

   pSettings = GetSettings();
   if (pSettings == NULL) {
      status = STATUS_INVALID_PARAMETER;

      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "RedirectFile: Settings are not set yet\n");

      goto Cleanup;
   }

   status = CopyUnicodeString(
      PagedPool,
      &usFileName,
      pNormalizedName,
      FSMON_TAG);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "RedirectFile: Failed copying %wZ: 0x%X\n",
         &pData->Iopb->TargetFileObject->FileName,
         status);

      goto Cleanup;
   }

   status = ReplaceUnicodeString(
      PagedPool,
      &usFileName,
      &pSettings->usTargetFolder,
      &pSettings->usRedirectFolder);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "RedirectFile: Failed replacing %wZ: 0x%X\n",
         &pData->Iopb->TargetFileObject->FileName,
         status);

      goto Cleanup;
   }

   status = UnicodeToFixString(&usFileName, &pFileName);
   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "RedirectFile: Failed replacing %wZ to fix string: 0x%X\n",
         &pData->Iopb->TargetFileObject->FileName,
         status);

      goto Cleanup;
   }
   status = IoReplaceFileObjectName(
      pData->Iopb->TargetFileObject,
      pFileName,
      usFileName.Length);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "RedirectFile: Failed IoReplaceFileObjectName %ws: 0x%X\n",
         pFileName,
         status);

      goto Cleanup;
   }
   status = STATUS_REPARSE;

Cleanup:
   FreeUnicodeString(&usFileName);
   if (pFileName) {
      ExFreePool(pFileName);
      pFileName = NULL;
   }

   if (status == STATUS_REPARSE) {
      pData->IoStatus.Status = status;
      pData->IoStatus.Information = IO_REPARSE;
      preOpStatus = FLT_PREOP_COMPLETE;
   }
   return preOpStatus;
}


NTSTATUS
UnicodeToFixString(
   _In_  PUNICODE_STRING   pUnicode,
   _Out_ WCHAR             **pString)
{
   NTSTATUS       status = STATUS_SUCCESS;

   NT_ASSERT(pUnicode);
   NT_ASSERT(pString);

   *pString = ExAllocatePoolWithTag(PagedPool, pUnicode->Length + sizeof(WCHAR), FSMON_TAG);
   if (*pString == NULL) {
      status = STATUS_INSUFFICIENT_RESOURCES;
      goto Cleanup;
   }
   RtlZeroMemory(*pString, pUnicode->Length + sizeof(WCHAR));
   RtlCopyMemory(*pString, pUnicode->Buffer, pUnicode->Length);

Cleanup:
   return status;
}

NTSTATUS
ReplaceUnicodeString(
   _In_     POOL_TYPE         poolType,
   _Inout_  PUNICODE_STRING   pusString,
   _In_     PCUNICODE_STRING  pusSearch,
   _In_     PCUNICODE_STRING  pusReplace)
{
   NTSTATUS          status = STATUS_SUCCESS;
   LONG              lPos = 0;
   USHORT            usPos = 0;
   USHORT            usLength = 0;
   WCHAR             *pString = NULL;
   CHAR              *pPtr = NULL;
   CHAR              *pPtrBuffer = NULL;

   status = FindString(pusString, pusSearch, &lPos, FALSE);
   if (!NT_SUCCESS(status)) {
      goto Cleanup;
   }

   if (lPos != -1) {
      usLength = pusString->Length - pusSearch->Length + pusReplace->Length + 1;
      pString = ExAllocatePoolWithTag(poolType, usLength * sizeof(WCHAR), FSMON_TAG);
      if (pString == NULL) {
         status = STATUS_INSUFFICIENT_RESOURCES;
         goto Cleanup;
      }
      RtlZeroMemory(pString, usLength);

      usPos = (USHORT)(lPos * sizeof(WCHAR));

      pPtr = (CHAR *)pString;
      pPtrBuffer = (CHAR *)pusString->Buffer;

      RtlCopyMemory(pPtr, pPtrBuffer, usPos);

      pPtr += usPos;
      RtlCopyMemory(pPtr, pusReplace->Buffer, pusReplace->Length);

      pPtr += pusReplace->Length;
      RtlCopyMemory(
         pPtr,
         pPtrBuffer + pusSearch->Length + usPos,
         pusString->Length - usPos - pusSearch->Length);

      ExFreePool(pusString->Buffer);
      pusString->Buffer = pString;

      pusString->MaximumLength = usLength;
      pusString->Length = (USHORT)(wcslen(pString) * sizeof(WCHAR));
   }
   else {
      status = STATUS_FAIL_CHECK;
   }
Cleanup:
   return status;
}

BOOLEAN
NeedRedirect(
   _In_  PFLT_CALLBACK_DATA      pData,
   _Out_ PUNICODE_STRING         pusNormalizeName)
{
   BOOLEAN                    bRes = FALSE;
   NTSTATUS                   status = STATUS_SUCCESS;
   PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
   PFSMON_SETTINGS            pSettings = NULL;
   LONG                       lPos = -1;

   if (pData && pusNormalizeName) {
      pSettings = GetSettings();
      if (!pSettings) {
         goto Cleanup;
      }

      status = FltGetFileNameInformation(
         pData,
         FLT_FILE_NAME_OPENED | /*FLT_FILE_NAME_QUERY_DEFAULT,*/FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY,
         &pNameInfo);

      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }

      status = FltParseFileNameInformation(pNameInfo);
      if (!NT_SUCCESS(status)) {
         goto Cleanup;
      }

      status = FindString(&pNameInfo->Name, &pSettings->usTargetFolder, &lPos, FALSE);
      if (lPos != -1) {
         CopyUnicodeString(NonPagedPool, pusNormalizeName, &pNameInfo->Name, FSMON_TAG);
         bRes = TRUE;
      }
   }

Cleanup:
   if (pNameInfo) {
      FltReleaseFileNameInformation(pNameInfo);
   }
   return bRes;
}

NTSTATUS
NormalizeFileName(
   _In_  PCWSTR      pFileName,
   _In_  ULONG       ulFileNameLength,
   _Out_ PWSTR       *pNormalized)
{
   NTSTATUS             status = STATUS_SUCCESS;
   SIZE_T               stVolumeLength = 0;
   SIZE_T               stPathLength = 0;
   PWSTR                pPtr = NULL;
   PWSTR                pPath = NULL;
   PWSTR                pOriginalFileName = NULL;
   WCHAR                sDrive[2] = { L'\0' };
   PVOLUME_MAP          pVolumeMap = NULL;
   PLIST_ENTRY          pEntry = NULL;
   PLIST_ENTRY          pVolumeMapHeader = NULL;
   PFAST_MUTEX          pVolumeMapMutex = NULL;

   pOriginalFileName = ExAllocatePool(PagedPool, ulFileNameLength + sizeof(WCHAR));
   if (pOriginalFileName == NULL) {
      status = STATUS_INSUFFICIENT_RESOURCES;
      goto Cleanup;
   }
   RtlZeroMemory(pOriginalFileName, ulFileNameLength + sizeof(WCHAR));
   RtlCopyMemory(pOriginalFileName, pFileName, ulFileNameLength);

   pPtr = wcsstr(pOriginalFileName, L"??");
   if (pPtr) {
      pPtr = wcsstr(pPtr, L":");
      if (pPtr) {
         pPtr--;
         sDrive[0] = pPtr[0];

         pVolumeMapHeader = GetVolumeMapHeader();
         pVolumeMapMutex = GetVolumeMapMutex();
         if (pVolumeMapHeader == NULL || pVolumeMapMutex == NULL) {
            status = STATUS_FAIL_CHECK;
            goto Cleanup;
         }

         ExAcquireFastMutex(pVolumeMapMutex);

         pEntry = pVolumeMapHeader->Flink;
         while (pEntry != NULL && pEntry != pVolumeMapHeader) {
            pVolumeMap = CONTAINING_RECORD(pEntry, VOLUME_MAP, Entry);
            if (wcsstr(pVolumeMap->MountName, sDrive)) {
               pPath = pPtr;
               pPath += 2;

               stPathLength = wcslen(pPath);
               stVolumeLength = wcslen(pVolumeMap->VolumeName);

               *pNormalized = ExAllocatePool(PagedPool, (stPathLength + stVolumeLength + 1) * sizeof(WCHAR));
               if (*pNormalized == NULL) {
                  status = STATUS_INSUFFICIENT_RESOURCES;
                  goto Cleanup;
               }

               RtlZeroMemory(*pNormalized, (stPathLength + stVolumeLength + 1) * sizeof(WCHAR));
               RtlCopyMemory(*pNormalized, pVolumeMap->VolumeName, stVolumeLength * sizeof(WCHAR));
               pPtr = *pNormalized;
               pPtr += stVolumeLength;
               RtlCopyMemory(pPtr, pPath, stPathLength * sizeof(WCHAR));

               break;
            }
            pEntry = pEntry->Flink;
         }
         ExReleaseFastMutex(pVolumeMapMutex);
      }
   }

Cleanup:
   if (!NT_SUCCESS(status)) {
      if (*pNormalized) {
         ExFreePool(*pNormalized);
      }
   }
   if (pOriginalFileName) {
      ExFreePool(pOriginalFileName);
   }
   return status;
}