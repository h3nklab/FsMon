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

    File name: init.h
    Contact: hdmih@yahoo.com
    Created: 05-07-2020
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#pragma once

EXTERN_C_START

NTSTATUS
InitializeSettings(
   _In_     PCUNICODE_STRING  pusRegistryPath,
   _Inout_  PFSMON_SETTINGS   *pSettings);

VOID
CleanupSettings(
   _In_  PFSMON_SETTINGS   *pSettings);

NTSTATUS
CreateSettings(
   _In_     POOL_TYPE         poolType,
   _Inout_  PFSMON_SETTINGS   *pSettings);

NTSTATUS
LoadRegistryString(
   _In_  HANDLE            hKey,
   _In_  WCHAR             *pName,
   _Out_ PUNICODE_STRING   pusValue);

EXTERN_C_END
