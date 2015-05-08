#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "kii.h"
#include "kii_def.h"

#include "kii-core/kii.h"
#include "kii_core_impl.h"

int kii_object_create(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_data,
		const char* object_content_type,
		char* out_object_id)
{
	char* p1;
	char* p2;
	char* buf;
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_create_new_object(
			kii,
			bucket,
			object_data,
			object_content_type);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}

	buf = kii->http_context.buffer;
	p1 = strstr(buf, "\"objectID\"");
	if(p1 == NULL) {
		goto exit;
	}
	p1 = strstr(p1, ":");
	if(p1 == NULL) {
		goto exit;
	}
	p1 = strstr(p1, "\"");
	if(p1 == NULL) {
		goto exit;
	}
	p1 += 1;
	p2 = strstr(p1, "\"");
	if(p2 == NULL) {
		goto exit;
	}
	memcpy(out_object_id, p1, p2 - p1);
	ret = 0;

exit:
	return ret;
}

int kii_object_create_with_id(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* object_data,
		const char* object_content_type)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_create_new_object_with_id(
			kii,
			bucket,
			object_id,
			object_data,
			object_content_type);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}

	ret = 0;

exit:
	return ret;

}

int kii_object_patch(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* patch_data,
		const char* opt_etag)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_patch_object(
			kii,
			bucket,
			object_id,
			patch_data,
			opt_etag);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}

	ret = 0;

exit:
	return ret;	
}

int kii_object_replace(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* replacement_data,
		const char* opt_etag)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_replace_object(
			kii,
			bucket,
			object_id,
			replacement_data,
			opt_etag);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}

	ret = 0;

exit:
	return ret;	
}

int kii_object_delete(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_delete_object(
			kii,
			bucket,
			object_id);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}

	ret = 0;
exit:
	return ret;
}

int kii_object_get(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;

	core_err = kii_get_object(
			kii,
			bucket,
			object_id);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	ret = 0;
exit:
	return ret;
}

int kii_object_upload_body_at_once(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* body_content_type,
		const char* data,
		size_t data_length)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body");
	core_err = kii_api_call(
			kii,
			"PUT",
			resource_path,
			data,
			body_content_type,
			NULL);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	ret = 0;
exit:
	return ret;
}

int kii_object_init_upload_body(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		char* out_upload_id)
{ 
	char* p1;
	char* p2;
	char* buf;
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body/uploads");
	core_err = kii_api_call(
			kii,
			"POST",
			resource_path,
			"{}",
			"application/vnd.kii.startobjectbodyuploadrequest+json",
			"accept:application/vnd.kii.startobjectbodyuploadresponse+json",
			(char*)(0));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	buf = kii->http_context.buffer;
	p1 = strstr(buf, "\"uploadID\"");
	if(p1 == NULL) {
		goto exit;
	}
	p1 = strstr(p1, ":");
	if(p1 == NULL) {
		goto exit;
	}
	p1 = strstr(p1, "\"");
	if(p1 == NULL) {
		goto exit;
	}
	p1 += 1;
	p2 = strstr(p1, "\"");
	if(p2 == NULL) {
		goto exit;
	}
	strncpy(out_upload_id, p1, p2 - p1);
	ret = 0;
exit:
	return ret;
}

int kii_object_upload_body(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* upload_id,
		const kii_chunk_data_t* chunk)
{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];
	char content_range[128];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body/uploads/");
	strcat(resource_path, upload_id);
	strcat(resource_path, "/data");

	/* content-range */
	memset(content_range, 0x00, sizeof(content_range));
	strcpy(content_range, STR_CONTENT_RANGE);
	strcat(content_range, "bytes=");
	sprintf(content_range + strlen(content_range), "%d", chunk->position);
	strcat(content_range, "-");
	sprintf(content_range + strlen(content_range), "%d", chunk->position+ chunk->length- 1);
	strcat(content_range, "/");
	sprintf(content_range + strlen(content_range), "%d", chunk->total_length);

	core_err = kii_api_call(
			kii,
			"PUT",
			resource_path,
			chunk->chunk,
			chunk->body_content_type,
			"Accept: application/json, application/*+json",
			content_range,
			(char*)(0));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	ret = 0;
exit:
	return ret;
}

int kii_object_commit_upload(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		const char* upload_id,
		unsigned int commit)

{
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body/uploads/");
	strcat(resource_path, upload_id);
	strcat(resource_path, "/status/");
	if(commit == 0)	{
		strcat(resource_path, "cancelled");
	}
	else {
		strcat(resource_path, "committed");
	}

	core_err = kii_api_call(
			kii,
			"POST",
			resource_path,
			NULL,
			NULL,
			NULL);
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	ret = 0;
exit:
	return ret;
}

int kii_object_download_body_at_once(
		kii_t* kii,
		const kii_bucket_t* bucket,
		const char* object_id,
		unsigned int* out_data_length)

{
	char* p1;
	char* buf;
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body");

	core_err = kii_api_call(
			kii,
			"GET",
			resource_path,
			NULL,
			NULL,
			"Accept: */*",
			(char*)(0));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	buf = kii->http_context.buffer;
	p1 = strstr(buf, STR_CONTENT_LENGTH);
	if(p1 == NULL)	{
		goto exit;
	}
	p1 = p1 + strlen(STR_CONTENT_LENGTH);
	*out_data_length = atoi(p1);
	ret = 0;
exit:
	return ret;
}

int kii_object_downlad_body(
		kii_t* kii,
		const char* object_id,
		const kii_bucket_t* bucket,
		unsigned int position,
		unsigned int length,
		unsigned int* out_actual_length,
		unsigned int* out_total_length)
{
	char* p1;
	char* buf;
	int ret = -1;
	kii_error_code_t core_err;
	kii_state_t state;
	char resource_path[256];
	char range[128];

	memset(resource_path, 0x00, sizeof(resource_path));
	strcpy(resource_path, "api/apps/");
	strcat(resource_path, kii->app_id);
	if(bucket->scope == KII_SCOPE_THING) {
		strcat(resource_path, "/things/");
		strcat(resource_path, bucket->scope_id);
	}
	strcat(resource_path, "/buckets/");
	strcat(resource_path, bucket->bucket_name);
	strcat(resource_path, "/objects/");
	strcat(resource_path, object_id);
	strcat(resource_path, "/body");

	/* Range */
	memset(range, 0x00, sizeof(range));
	strcpy(range, STR_RANGE);
	strcat(range, "bytes=");
	sprintf(range + strlen(range), "%d", position);
	strcat(range, "-");
	sprintf(range + strlen(range), "%d", position + length - 1);

	core_err = kii_api_call(
			kii,
			"GET",
			resource_path,
			NULL,
			NULL,
			range,
			"Accept: */*",
			(char*)(0));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	do {
		core_err = kii_run(kii);
		state = kii_get_state(kii);
	} while (state != KII_STATE_IDLE);
	M_KII_LOG(kii->logger_cb("resp: %s\n", kii->response_body));
	if (core_err != KIIE_OK) {
		goto exit;
	}
	if(kii->response_code < 200 || 300 <= kii->response_code) {
		goto exit;
	}
	buf = kii->http_context.buffer;
	p1 = strstr(buf, STR_CONTENT_RANGE);
	if(p1 == NULL)	{
		goto exit;
	}
	p1 = strstr(p1, "/");
	if(p1 == NULL) {
		goto exit;
	}
	p1++;
	*out_total_length = atoi(p1);

	p1 = strstr(buf, STR_CONTENT_LENGTH);
	if(p1 == NULL) {
		goto exit;
	}
	p1 = p1 + strlen(STR_CONTENT_LENGTH);
	*out_actual_length = atoi(p1);
	ret = 0;
exit:
	return ret;
}

