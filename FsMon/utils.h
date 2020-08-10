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


EXTERN_C_START

NTSTATUS
FindString(
   _In_  PCUNICODE_STRING  pusString,
   _In_  PCUNICODE_STRING  pusSearch,
   _In_  LONG              *plPos,
   _In_  BOOLEAN           bCaseSensitive);

NTSTATUS
CopyUnicodeString(
   _In_     POOL_TYPE         poolType,
   _Inout_  PUNICODE_STRING   pDest,
   _In_     PCUNICODE_STRING  pSrc,
   _In_     ULONG             tag);

BOOLEAN
EqualUnicodeString(
   _In_  PUNICODE_STRING   pusString1,
   _In_  PUNICODE_STRING   pusString2,
   _In_  BOOLEAN           bCaseSensitive);

VOID
FreeUnicodeString(
   _Inout_  PUNICODE_STRING   pString);

NTSTATUS
InitUnicodeFromString(
   _In_     POOL_TYPE         poolType,
   _Inout_  PUNICODE_STRING   pUnicode,
   _In_     WCHAR             *pString,
   _In_     ULONG             tag);

FLT_PREOP_CALLBACK_STATUS
RedirectFile(
   _In_  PFLT_CALLBACK_DATA   pData,
   _In_  PCUNICODE_STRING     pNormalizedName);

NTSTATUS
ReplaceUnicodeString(
   _In_     POOL_TYPE         poolType,
   _Inout_  PUNICODE_STRING   pusString,
   _In_     PCUNICODE_STRING  pusSearch,
   _In_     PCUNICODE_STRING  pusReplace);

NTSTATUS
UnicodeToFixString(
   _In_  PUNICODE_STRING   pUnicode,
   _Out_ WCHAR             **pString);

BOOLEAN
NeedRedirect(
   _In_  PFLT_CALLBACK_DATA      pData,
   _Out_ PUNICODE_STRING         pusNormalizeName);

NTSTATUS
NormalizeFileName(
   _In_  PCWSTR      pFileName,
   _In_  ULONG       ulFileNameLength,
   _Out_ PWSTR       *pNormalized);

EXTERN_C_END
