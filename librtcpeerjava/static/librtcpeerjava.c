// gcc -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -I/usr/lib/jvm/java-8-openjdk-amd64/include -o librtcpeerjava.so -shared librtcpeerjava.c -lrtcpeer
#include <jni.h>
#include "librtcpeer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>








// rm /usr/lib/librtcpeerjava.so
// cp librtcpeerjava.so /usr/lib/librtcpeerjava.so
// ldconfig

static const char *JNIT_CLASS = "RTCPeer";


jobject qcreatePeer(JNIEnv *env, jobject obj) {
	
	RTCPeer *peer = rtc_peer_new();
	
	rtc_init_base_channel(peer);
	
	rtc_init_ssl(peer);

	rtc_new_network_thread(peer);

	rtc_new_worker_thread(peer);

	rtc_new_signaling_thread(peer);

	rtc_new_factory_dependencies(peer);

	jobject ptr = (*env)->NewDirectByteBuffer(env, (void*)peer, sizeof(RTCPeer *));
	
	return ptr;
}


void qsetStunServer(JNIEnv *env, jobject obj, jobject ptr, jstring addr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	const char *path = (const char *)(*env)->GetStringUTFChars(env, addr, NULL);
	
	rtc_set_ice_server(peer, path);
}


void qcreatePeerConnection(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	rtc_new_peer_connection(peer);
}


void qcreateSessionDescription(JNIEnv *env, jobject obj, jobject ptr, jstring type) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	const char *typeStr = (const char *)(*env)->GetStringUTFChars(env, type, NULL);
	
	if(strcmp(typeStr, "offer") == 0) {
		
		rtc_new_sdp_offer(peer);
	}
	else  if(strcmp(typeStr, "answer") == 0) {
		
		rtc_new_sdp_answer(peer);
	}	
}


void qparseSessionDescription(JNIEnv *env, jobject obj, jobject ptr, jstring type, jstring sdp) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);

	const char *typeStr = (const char *)(*env)->GetStringUTFChars(env, type, NULL);
	const char *sdpStr = (const char *)(*env)->GetStringUTFChars(env, sdp, NULL);
	
	if(strcmp(typeStr, "offer") == 0) {
		
		rtc_parse_sdp_offer(peer, sdpStr);
	}
	else  if(strcmp(typeStr, "answer") == 0) {
		
		rtc_parse_sdp_answer(peer, sdpStr);
	}
}


void qaddIceCandidate(JNIEnv *env, jobject obj, jobject ptr, jstring ice) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	const char *iceStr = (const char *)(*env)->GetStringUTFChars(env, ice, NULL);
	
	rtc_add_ice_candidate(peer, iceStr);
}


void qcreateDataChannel(JNIEnv *env, jobject obj, jobject ptr, jstring name) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	const char *nameStr = (const char *)(*env)->GetStringUTFChars(env, name, NULL);
	
	rtc_new_data_channel(peer, nameStr);
}


jint qgetState(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	int statG = rtc_datachannel_get_state(peer);
	jint stat = (jint)statG;
	return stat;
}


jint qsendBinary(JNIEnv *env, jobject obj, jobject ptr, jbyteArray array, jint arraylen) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
	
	uint8_t buf[65000];
	
	memcpy(buf, bytes, (int)arraylen);
	
	(*env)->ReleaseByteArrayElements(env, array, bytes, 0);

	int len = rtc_datachannel_send_binary(peer, (char *)buf, (int)arraylen);

	return (jint)len;
}


jint qsend(JNIEnv *env, jobject obj, jobject ptr, jstring s1) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	const char *sendStr = (const char *)(*env)->GetStringUTFChars(env, s1, NULL);
	
	int len = rtc_datachannel_send_text(peer, (char *)sendStr, strlen(sendStr));
	
	return (jint)len;
}


jbyteArray qreadDebugMessage(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	jbyteArray array = (*env)->NewByteArray(env, 65000); 
	
	
	uint8_t buf[65000];
	int len = rtc_read_debug_message(peer, buf, sizeof(buf));
	if(len > 0) {
		
		int qlen = strlen(buf);
		if(qlen > 0) {
			buf[qlen] = '\0';	
			jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
			memcpy(bytes, buf, qlen);
			(*env)->SetByteArrayRegion(env, array, 0, qlen, bytes);
			(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
		}
	}
	
	return array;
}


jbyteArray qreadSessionDescription(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	jbyteArray array = (*env)->NewByteArray(env, 65000); 
	
	
	uint8_t buf[65000];
	int len = rtc_read_sdp(peer, buf, sizeof(buf));
	if(len > 0) {

		int qlen = strlen(buf);
		if(qlen > 0) {
			buf[qlen] = '\0';	
			jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
			memcpy(bytes, buf, qlen);
			(*env)->SetByteArrayRegion(env, array, 0, qlen, bytes);
			(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
		}
	}
	
	return array;
}


jbyteArray qreadIceCandidate(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	jbyteArray array = (*env)->NewByteArray(env, 65000); 
	
	
	uint8_t buf[65000];
	int len = rtc_read_ice_candidate(peer, buf, sizeof(buf));
	if(len > 0) {

		int qlen = strlen(buf);
		if(qlen > 0) {
			buf[qlen] = '\0';	
			jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
			memcpy(bytes, buf, qlen);
			(*env)->SetByteArrayRegion(env, array, 0, qlen, bytes);
			(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
		}
	}
	
	return array;
}


jbyteArray qreadBinary(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	jbyteArray array = (*env)->NewByteArray(env, 65000); 
	
	uint8_t buf[65000];
	int len = rtc_datachannel_read(peer, buf, sizeof(buf));
	if(len > 0) {

		jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
		memcpy(bytes, buf, sizeof(buf));
		(*env)->SetByteArrayRegion(env, array, 0, sizeof(buf), bytes);
		(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
	}
	
	return array;
}


jbyteArray qread(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	jbyteArray array = (*env)->NewByteArray(env, 65000); 
	
	uint8_t buf[65000];
	int len = rtc_datachannel_read(peer, buf, sizeof(buf));
	if(len > 0) {

		jbyte *bytes = (*env)->GetByteArrayElements(env, array, 0);
		memcpy(bytes, buf, sizeof(buf));
		(*env)->SetByteArrayRegion(env, array, 0, sizeof(buf), bytes);
		(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
	}
	
	return array;
}


void qclose(JNIEnv *env, jobject obj, jobject ptr) {
	
	RTCPeer *peer = (RTCPeer*)(*env)->GetDirectBufferAddress(env, ptr);
	
	rtc_del_network(peer);
	rtc_del_worker(peer);
	rtc_del_signaling(peer);
	rtc_del_peer_connection(peer);
	rtc_del_data_channel(peer);
	rtc_del_peer_connection_factory(peer);
	del_rtc_peer(peer);
}


static JNINativeMethod funcs[] = {
	{ "createPeerConnection", "(Ljava/lang/Object;)V", &qcreatePeerConnection },
	{ "createPeer", "()Ljava/lang/Object;", &qcreatePeer },
	{ "setStunServer", "(Ljava/lang/Object;Ljava/lang/String;)V", &qsetStunServer },
	{ "createSessionDescription", "(Ljava/lang/Object;Ljava/lang/String;)V", &qcreateSessionDescription },
	{ "parseSessionDescription", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V", &qparseSessionDescription },
	{ "addIceCandidate", "(Ljava/lang/Object;Ljava/lang/String;)V", &qaddIceCandidate },
	{ "createDataChannel", "(Ljava/lang/Object;Ljava/lang/String;)V", &qcreateDataChannel },
	{ "sendBinary", "(Ljava/lang/Object;[BI)I", &qsendBinary },
	{ "send", "(Ljava/lang/Object;Ljava/lang/String;)I", &qsend },
	{ "readDebugMessage", "(Ljava/lang/Object;)[B", &qreadDebugMessage },
	{ "readIceCandidate", "(Ljava/lang/Object;)[B", &qreadIceCandidate },
	{ "readSessionDescription", "(Ljava/lang/Object;)[B", &qreadSessionDescription },
	{ "readBinary", "(Ljava/lang/Object;)[B", &qreadBinary },
	{ "read", "(Ljava/lang/Object;)[B", &qread },
	{ "getState", "(Ljava/lang/Object;)I", &qgetState },
	{ "close", "(Ljava/lang/Object;)V", &qclose },
};


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	
    JNIEnv *env;
    jclass  cls;
    jint    res;

    (void)reserved;

    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_8) != JNI_OK)
        return -1;

    cls = (*env)->FindClass(env, JNIT_CLASS);
    if (cls == NULL)
        return -1;

    res = (*env)->RegisterNatives(env, cls, funcs, sizeof(funcs)/sizeof(*funcs));
    if (res != 0)
        return -1;

    return JNI_VERSION_1_8;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	
    JNIEnv *env;
    jclass  cls;

    (void)reserved;

    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_8) != JNI_OK)
        return;

    cls = (*env)->FindClass(env, JNIT_CLASS);
    if (cls == NULL)
        return;

    (*env)->UnregisterNatives(env, cls);
}



