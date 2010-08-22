/*
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef ANDROID

#include "jnihelpers.h"

jobject VariantToJObject(Variant &value, JNIEnv* pEnv) {
	pEnv->PushLocalFrame(128);
	switch ((VariantType) value) {
		case V_NULL:
		case V_UNDEFINED:
		{
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
		case V_BOOL:
		{
			jclass clazz = pEnv->FindClass("java/lang/Boolean");
			jmethodID constructor = pEnv->GetMethodID(clazz, "<init>", "(Z)V");
			pEnv->PopLocalFrame(NULL);
			return pEnv->NewObject(clazz, constructor, (bool)value);
		}
		case V_INT8:
		case V_INT16:
		case V_INT32:
		case V_UINT8:
		case V_UINT16:
		{
			jclass clazz = pEnv->FindClass("java/lang/Integer");
			jmethodID constructor = pEnv->GetMethodID(clazz, "<init>", "(I)V");
			pEnv->PopLocalFrame(NULL);
			return pEnv->NewObject(clazz, constructor, (int32_t) value);
		}
		case V_INT64:
		case V_UINT32:
		{
			jclass clazz = pEnv->FindClass("java/lang/Long");
			jmethodID constructor = pEnv->GetMethodID(clazz, "<init>", "(J)V");
			pEnv->PopLocalFrame(NULL);
			return pEnv->NewObject(clazz, constructor, (int64_t) value);
		}
		case V_UINT64:
		{
			NYI;
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
		case V_DOUBLE:
		{
			jclass clazz = pEnv->FindClass("java/lang/Double");
			jmethodID constructor = pEnv->GetMethodID(clazz, "<init>", "(D)V");
			pEnv->PopLocalFrame(NULL);
			return pEnv->NewObject(clazz, constructor, (double) value);
		}
		case V_TIMESTAMP:
		case V_DATE:
		case V_TIME:
		{
			NYI;
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
		case V_BYTEARRAY:
		{
			NYI;
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
		case V_STRING:
		{
			pEnv->PopLocalFrame(NULL);
			return pEnv->NewStringUTF(STR(value));
		}
		case V_TYPED_MAP:
		{
			NYI;
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
		case V_MAP:
		{
			jclass mapClazz = pEnv->FindClass("java/util/HashMap");
			jmethodID mapConstructor = pEnv->GetMethodID(mapClazz, "<init>", "()V");
			jmethodID mapPut = pEnv->GetMethodID(mapClazz, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

			jclass longClazz = pEnv->FindClass("java/lang/Long");
			jmethodID longConstructor = pEnv->GetMethodID(longClazz, "<init>", "(J)V");

			jobject result = pEnv->NewObject(mapClazz, mapConstructor);

			FOR_MAP(value, string, Variant, i) {
				jobject key = NULL;
				if (MAP_KEY(i).find("__index__value__") == 0) {
					key = pEnv->NewObject(longClazz, longConstructor,
							(int64_t) atol(STR(MAP_KEY(i).substr(16))));
				} else {
					key = pEnv->NewStringUTF(STR(MAP_KEY(i)));
				}
				jobject value = VariantToJObject(MAP_VAL(i), pEnv);
				pEnv->CallObjectMethod(result, mapPut, key, value);
			}

			pEnv->PopLocalFrame(NULL);
			return result;
		}
		default:
		{
			FATAL("Invalid variant type: %d", (VariantType) value);
			pEnv->PopLocalFrame(NULL);
			return NULL;
		}
	}
}

bool CallJava(CallBackInfo &ci, Variant &parameters) {
	if (ci.pEnv == NULL) {
		WARN("No pEnv");
		return true;
	}
	if (ci.pInterface == NULL) {
		WARN("No pInterface");
		return true;
	}
	ci.pEnv->PushLocalFrame(128);
	if (ci.method == NULL) {
		ci.clazz = ci.pEnv->FindClass("com/rtmpd/VideoCallbacks");
		if (ci.clazz != NULL) {
			ci.method = ci.pEnv->GetMethodID(ci.clazz, "EventAvailable", "(Ljava/lang/Object;)V");
		}
	}
	ci.pEnv->CallObjectMethod(ci.pInterface, ci.method, VariantToJObject(parameters, ci.pEnv));
	ci.pEnv->PopLocalFrame(NULL);
	return true;
}

#endif /* ANDROID */