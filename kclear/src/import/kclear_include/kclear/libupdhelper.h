// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

class __declspec(uuid("00140123-5CCE-4279-BD9B-11D66677BB76"))
IUpdateHelper : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Combine( LPCWSTR lpwszDifflib ) = 0;
    virtual HRESULT STDMETHODCALLTYPE Notify( LPCWSTR lpwszFileName ) = 0;
};

#define FN_CreateUpdateHelper			"CreateUpdateHelper"
typedef HRESULT (WINAPI *PFN_CreateUpdateHelper)( REFIID riid, void** ppvObj );
