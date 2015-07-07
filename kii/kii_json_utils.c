#include "kii_json_utils.h"

#include <string.h>

kii_json_parse_result_t prv_kii_json_read_object(
        kii_t* kii,
        const char* json_string,
        size_t json_string_size,
        kii_json_field_t *fields)
{
    kii_json_t kii_json;
    kii_json_parse_result_t retval;

    memset(&kii_json, 0, sizeof(kii_json));

#ifdef KII_JSON_STACK_TOKEN_NUM
    {
        kii_json_token_t tokens[KII_JSON_STACK_TOKEN_NUM];
        kii_json.tokens = tokens;
        kii_json.json_token_num = sizeof(tokens) / sizeof(tokens[0]);
        retval = kii_json_read_object(&kii_json, json_string, json_string_size,
                fields);
    }
#else
    kii_json.tokens = kii->kii_json_resource->tokens;
    kii_json.json_token_num = kii->kii_json_resource->tokens_num;
    retval = kii_json_read_object(&kii_json, json_string, json_string_size,
                fields);
    if (kii->kii_json_resource_cb != NULL &&
            retval == KII_JSON_PARSE_SHORTAGE_TOKENS) {
        if (kii->kii_json_resource_cb(kii->kii_json_resource,
                        kii_json.json_token_num) != KIIE_OK) {
            return KII_JSON_PARSE_SHORTAGE_TOKENS;
        }
        kii_json.tokens = kii->kii_json_resource->tokens;
        kii_json.json_token_num = kii->kii_json_resource->tokens_num;
        retval = kii_json_read_object(&kii_json, json_string,
                json_string_size, fields);
    }
#endif
    return retval;
}

