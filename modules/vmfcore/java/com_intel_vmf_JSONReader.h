/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_intel_vmf_JSONReader */

#ifndef _Included_com_intel_vmf_JSONReader
#define _Included_com_intel_vmf_JSONReader
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_JSONReader
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_intel_vmf_JSONReader_n_1JSONReader
  (JNIEnv *, jobject);

/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_parseAll
 * Signature: (JLjava/lang/String;JLjava/lang/String;Ljava/lang/String;[Lcom/intel/vmf/MetadataStream/VideoSegment;[Lcom/intel/vmf/MetadataSchema;[Lcom/intel/vmf/MetadataInternal;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_vmf_JSONReader_n_1parseAll
  (JNIEnv *, jclass, jlong, jstring, jlong, jstring, jstring, jobjectArray, jobjectArray, jobjectArray);

/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_parseSchemas
 * Signature: (JLjava/lang/String;[Lcom/intel/vmf/MetadataSchema;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_vmf_JSONReader_n_1parseSchemas
  (JNIEnv *, jclass, jlong, jstring, jobjectArray);

/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_parseMetadata
 * Signature: (JLjava/lang/String;[Lcom/intel/vmf/MetadataSchema;[Lcom/intel/vmf/MetadataInternal;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_vmf_JSONReader_n_1parseMetadata
  (JNIEnv *, jclass, jlong, jstring, jobjectArray, jobjectArray);

/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_parseVideoSegments
 * Signature: (JLjava/lang/String;[Lcom/intel/vmf/MetadataStream/VideoSegment;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_vmf_JSONReader_n_1parseVideoSegments
  (JNIEnv *, jclass, jlong, jstring, jobjectArray);

/*
 * Class:     com_intel_vmf_JSONReader
 * Method:    n_delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_intel_vmf_JSONReader_n_1delete
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif