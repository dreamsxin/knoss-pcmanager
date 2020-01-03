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

/********************************************************************
	created:	2010/06/05
	created:	5:6:2010   17:44
	filename: 	tinyxml_wrapper.cpp
	author:		Jiang Fengbing
	
	purpose:	tinyxml的包裹，用来消除警告
*********************************************************************/

#pragma warning(push)
#pragma warning(disable: 6011)
#pragma warning(disable: 6031)
#pragma warning(disable: 6244)

#include "tinystr.cpp"
#include "tinyxmlerror.cpp"
#include "tinyxmlparser.cpp"
#include "tinyxml.cpp"

#pragma warning(pop)
