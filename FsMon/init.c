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

    File name: init.c
    Contact: hdmih@yahoo.com
    Created: 05-07-2020
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <fltKernel.h>
#include <dontuse.h>

#include "struct.h"
#include "utils.h"
#include "init.h"

VOID
CleanupSettings(
   _In_  PFSMON_SETTINGS   *pSettings)
{
   if (pSettings && *pSettings) {
      FreeUnicodeString(&(*pSettings)->usTargetFolder);
      FreeUnicodeString(&(*pSettings)->usRedirectFolder);

      ExFreePool(*pSettings);
   }
   *pSettings = NULL;
}

NTSTATUS
CreateSettings(
   _In_     POOL_TYPE         poolType,
   _Inout_  PFSMON_SETTINGS   *pSettings)
{
   NTSTATUS       status = STATUS_SUCCESS;

   CleanupSettings(pSettings);

   *pSettings = ExAllocatePoolWithTag(
      poolType,
      sizeof(FSMON_SETTINGS),
      FSMON_TAG);

   if (*pSettings == NULL) {
      status = STATUS_INSUFFICIENT_RESOURCES;
      goto Cleanup;
   }

   RtlZeroMemory(*pSettings, sizeof(FSMON_SETTINGS));

Cleanup:
   return status;
}

NTSTATUS
InitializeSettings(
   _In_     PCUNICODE_STRING  pusRegistryPath,
   _Inout_  PFSMON_SETTINGS   *pSettings)
{
   NTSTATUS          status = STATUS_SUCCESS;
   OBJECT_ATTRIBUTES oa = { 0 };
   HANDLE            hReg = NULL;

   status = CreateSettings(NonPagedPoolNx, pSettings);
   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "InitializeSettings: Memory exhausted!!!\n");
      goto Cleanup;
   }

   InitializeObjectAttributes(
      &oa,
      (PUNICODE_STRING)pusRegistryPath,
      OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
      NULL,
      NULL);

   status = ZwOpenKey(&hReg, KEY_READ | KEY_QUERY_VALUE, &oa);
   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "InitializeSettings: Failed opening registry %wZ: 0x%X\n",
         pusRegistryPath,
         status);

      goto Cleanup;
   }

   status = LoadRegistryString(hReg, L"TargetFolder", &(*pSettings)->usTargetFolder);
   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "InitializeSettings: Failed reading \"TargetFolder\" registry: 0x%X\n",
         status);

      goto Cleanup;
   }

   status = LoadRegistryString(hReg, L"RedirectFolder", &(*pSettings)->usRedirectFolder);
   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "InitializeSettings: Failed reading \"RedirectFolder\" registry: 0x%X\n",
         status);

      goto Cleanup;
   }

Cleanup:
   if (hReg) {
      ZwClose(hReg);
      hReg = NULL;
   }
   return status;
}

NTSTATUS
LoadRegistryString(
   _In_  HANDLE            hKey,
   _In_  WCHAR             *pName,
   _Out_ PUNICODE_STRING   pusValue)
{
   NTSTATUS                         status = STATUS_SUCCESS;
   UNICODE_STRING                   usName = { 0 };
   PKEY_VALUE_PARTIAL_INFORMATION   pPartialValue = NULL;
   ULONG                            ulValueLength = 0;

   FLT_ASSERT(pusValue);

   status = InitUnicodeFromString(
      NonPagedPoolNx,
      &usName,
      pName,
      FSMON_TAG);

   status = ZwQueryValueKey(
      hKey,
      &usName,
      KeyValuePartialInformation,
      NULL,
      0,
      &ulValueLength);

   if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "LoadRegistryString: Failed querying \"%wZ\" registry: 0x%X\n",
         &usName,
         status);

      goto Cleanup;
   }

   pPartialValue = ExAllocatePoolWithTag(PagedPool, ulValueLength, FSMON_TAG);
   if (pPartialValue == NULL) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "LoadRegistryString: Failed allocting %d bytes of memory\n",
         ulValueLength);

      goto Cleanup;
   }

   status = ZwQueryValueKey(
      hKey,
      &usName,
      KeyValuePartialInformation,
      pPartialValue,
      ulValueLength,
      &ulValueLength);

   if (!NT_SUCCESS(status)) {
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "LoadRegistryString: Failed querying \"%wZ\" registry: 0x%X\n",
         &usName,
         status);
   }

   ulValueLength = pPartialValue->DataLength;

   pusValue->Buffer = ExAllocatePoolWithTag(
      NonPagedPoolNx,
      ulValueLength,
      FSMON_TAG);

   if (pusValue->Buffer == NULL) {
      status = STATUS_INSUFFICIENT_RESOURCES;
      DbgPrintEx(
         DPFLTR_DEFAULT_ID,
         0xFFFFFFFF,
         "LoadRegistryString: Failed allocting %d bytes of memory\n",
         ulValueLength);

      goto Cleanup;
   }
   pusValue->Length = (USHORT)(ulValueLength - sizeof(WCHAR));
   pusValue->MaximumLength = (USHORT)ulValueLength;

   RtlZeroMemory(pusValue->Buffer, ulValueLength);
   RtlCopyMemory(pusValue->Buffer, pPartialValue->Data, pusValue->Length);

Cleanup:
   if (!NT_SUCCESS(status)) {
      if (pusValue->Buffer) {
         ExFreePool(pusValue->Buffer);
         RtlZeroMemory(pusValue, sizeof(UNICODE_STRING));
      }
   }
   FreeUnicodeString(&usName);

   return status;
}

