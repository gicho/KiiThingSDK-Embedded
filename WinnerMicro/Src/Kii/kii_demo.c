#include <string.h>
#include "wm_include.h"

#include "kii.h"

#if 0
#define STR_SITE "CN"
#define STR_APPID "41532a0f"
#define STR_APPKEY "72894abd7ab2f3fee1ec9814976e57c6"
#else
#define STR_SITE "JP"
#define STR_APPID "f25bd5bf"
#define STR_APPKEY "3594109968d7adf522c9991c0be51137"
#endif
#if 0
#define STR_BUCKET "DevLedControl"
#define STR_APP_BUCKET "AppLedControl"
#define STR_MEDIA_TYPE "Led"
#define STR_OBJECTBODY_TYPE "Firmware/bin"
#define STR_JSONOBJECT "{\"Led\":\"On\"}"
#define STR_JSONOBJECT_PARTIALLY_UPDATE "{\"Led\":\"On\"}"
#define STR_JSONOBJECT_FULLY_UPDATE "{\"Led\":\"On\"}"
#define STR_OBJECT_ID "WinnerMicroTestObj"
#define DEVICE_TYPE "Led"
#define PASSWORD "123456"
#endif
#define STR_BUCKET_FIRWAREUPGRADE "WM_FirmwareUpgrade"
#define STR_BUCKET_FROM_SERVER "LedControl"
#define STR_BUCKET_TO_SERVER "LedState"
#define STR_MEDIA_TYPE "Led"
#define STR_OBJECTBODY_TYPE "Firmware/bin"
#define STR_JSONOBJECT "{\"Led\":\"On\"}"
#define DEVICE_TYPE "Led"
#define PASSWORD "123456"

#define STR_THING_TOPIC   "WinnerMicro_Led"
#if 0
void kiiDemo_testObject(void)
{
    char objectID[KII_OBJECTID_SIZE+1];
    static unsigned char objectMultiplePiecesBody1[20];
    static unsigned char objectMultiplePiecesBody2[30];
    static unsigned char objectBody[20];

    printf("kii demo object test.\r\n");

    if (kiiObj_create(STR_BUCKET, STR_JSONOBJECT, NULL, objectID) < 0)
    {
        printf("kii create object without data type failed !\r\n");
    }
    else
    {
        printf("kii object is created without data type, objectID:\"%s\"\r\n", objectID);
    }

    if (kiiObj_create(STR_BUCKET, STR_JSONOBJECT, STR_MEDIA_TYPE, objectID) < 0)
    {
        printf("kii create object with data type failed !\r\n");
    }
    else
    {
        printf("kii object is created with data type, objectID:\"%s\"\r\n", objectID);
    }

    if (kiiObj_createWithID(STR_BUCKET, STR_JSONOBJECT, NULL, STR_OBJECT_ID) < 0)
    {
        printf("kii object with ID and without data type failed !\r\n");
    }
    else
    {
        printf("kii object is created with ID and without data type , objectID:\"%s\"\r\n", STR_OBJECT_ID);
    }

    if (kiiObj_createWithID(STR_BUCKET, STR_JSONOBJECT, STR_MEDIA_TYPE, STR_OBJECT_ID) < 0)
    {
        printf("kii object with ID and data type failed !\r\n");
    }
    else
    {
        printf("kii object is created with ID and data type, objectID:\"%s\"\r\n", STR_OBJECT_ID);
    }

    if (kiiObj_partiallyUpdate(STR_BUCKET, STR_JSONOBJECT_PARTIALLY_UPDATE, objectID) < 0)
    {
        printf("kii object partially update failed !\r\n");
    }
    else
    {
        printf("kii object is partially updated, objectID:\"%s\"\r\n", objectID);
    }
	
    if (kiiObj_fullyUpdate(STR_BUCKET, STR_JSONOBJECT_FULLY_UPDATE, NULL,  objectID) < 0)
    {
        printf("kii object fully update without data type failed !\r\n");
    }
    else
    {
        printf("kii object is fully updated without data type, objectID:\"%s\"\r\n", objectID);
    }


    if (kiiObj_fullyUpdate(STR_BUCKET, STR_JSONOBJECT_FULLY_UPDATE, STR_MEDIA_TYPE,  objectID) < 0)
    {
        printf("kii object fully update with data type failed !\r\n");
    }
    else
    {
        printf("kii object is fully updated with data type, objectID:\"%s\"\r\n", objectID);
    }
	
    if (kiiObj_fullyUpdate(STR_BUCKET, STR_JSONOBJECT_FULLY_UPDATE, STR_MEDIA_TYPE,  objectID) < 0)
    {
        printf("kii object fully update with data type failed !\r\n");
    }
    else
    {
        printf("kii object is fully updated with data type, objectID:\"%s\"\r\n", objectID);
    }

    memset(objectBody, 'T', sizeof(objectBody));
    if (kiiObj_uploadBodyAtOnce(STR_BUCKET, objectID,  STR_OBJECTBODY_TYPE, objectBody, sizeof(objectBody)) < 0)
    {
        printf("kii object upload body at once  failed !\r\n");
    }
    else
    {
        printf("kii object body is uploaded at once, objectID:\"%s\"\r\n", objectID);
    }


    memset(objectMultiplePiecesBody1, 'U', sizeof(objectMultiplePiecesBody1));
    memset(objectMultiplePiecesBody2, 'E', sizeof(objectMultiplePiecesBody2));
    if (kiiObj_uploadBodyInit(STR_BUCKET, objectID, STR_OBJECTBODY_TYPE, sizeof(objectMultiplePiecesBody1)+sizeof(objectMultiplePiecesBody2)) < 0)
    {
        printf("kii object upload mutiple pieces body init failed !\r\n");
    }
    else if (kiiObj_uploadBody(objectMultiplePiecesBody1, sizeof(objectMultiplePiecesBody1)) < 0)
    {
        printf("kii object upload mutiple pieces of 1 failed !\r\n");
    }
    else if (kiiObj_uploadBody(objectMultiplePiecesBody2, sizeof(objectMultiplePiecesBody2)) < 0)
    {
        printf("kii object upload mutiple pieces of 2 failed !\r\n");
    }
    else if (kiiObj_uploadBodyCommit(1) < 0)
    {
        printf("kii object upload multiple pieces body commit failed !\r\n");
    }
    else
    {
        printf("kii object upload multiple pieces body successfully !\r\n");
    }

    if (kiiObj_uploadBodyInit(STR_BUCKET, objectID, STR_OBJECTBODY_TYPE, sizeof(objectMultiplePiecesBody1)+sizeof(objectMultiplePiecesBody2)) < 0)
    {
        printf("kii object upload mutiple pieces body init failed !\r\n");
    }
    else if (kiiObj_uploadBody(objectMultiplePiecesBody1, sizeof(objectMultiplePiecesBody1)) < 0)
    {
        printf("kii object upload mutiple pieces of 1 failed !\r\n");
    }
    else if (kiiObj_uploadBodyCommit(0) < 0)
    {
        printf("kii object upload multiple pieces body cancelled failed !\r\n");
    }
    else
    {
        printf("kii object upload multiple pieces body cancelled successfully !\r\n");
    }
}
#endif
void kiiDemo_pushMessageCallback(char* jsonBuf, int rcvdCounter)
{
    int i;
    char * p1;
    char * p2;

    char objectID[KII_OBJECTID_SIZE+1];
    char bucketName[KII_BUCKET_NAME_SIZE+1];


    p1 = strstr(jsonBuf, "objectID");
    if (p1 != NULL)
    {
        p1 +=8+3;
        p2 = strstr(p1, "\"");
	memset(objectID, 0 ,sizeof(objectID));
	memcpy(objectID, p1, p2-p1);
    }

    p1 = strstr(jsonBuf, "bucketID");
    if (p1 != NULL)
    {
        p1 +=8+3;
        p2 = strstr(p1, "\"");
	memset(bucketName, 0 ,sizeof(bucketName));
	memcpy(bucketName, p1, p2-p1);
    }

      printf("bucketID:%s\r\n", bucketName);
      printf("objectID:%s\r\n", objectID);

	if (strcmp(bucketName, STR_BUCKET_FIRWAREUPGRADE) == 0)
	{
	        printf("firmware upgrade... !\r\n");
	}
	else if (strcmp(bucketName, STR_BUCKET_FROM_SERVER) == 0)
	{
/*	
	    if (kiiObj_create(STR_BUCKET_TO_SERVER, STR_JSONOBJECT, STR_MEDIA_TYPE, objectID) < 0)
	    {
	        printf("kii create object with data type failed !\r\n");
	    }
	    else
	    {
	        printf("kii object is created with data type, objectID:\"%s\"\r\n", objectID);
	    }
*/
	    unsigned char mac_addr[8];
	    char vendorID[KII_DEVICE_VENDOR_ID+1] ;
	    int i;	

	    printf("Device on boarding ...\r\n");
	    memset(mac_addr,0,sizeof(mac_addr));
	    tls_get_mac_addr(mac_addr);
	    memset(vendorID, 0, sizeof(vendorID));
	    for(i=0; i<6; i++)
	    {
	        sprintf(vendorID+strlen(vendorID), "%02x", mac_addr[i]);
	    }

	    if (kiiObj_createWithID(STR_BUCKET_TO_SERVER, STR_JSONOBJECT, STR_MEDIA_TYPE, vendorID) < 0)
	    {
	        printf("kii object with ID and data type failed !\r\n");
	    }
	    else
	    {
	        printf("kii object is created with ID and data type, objectID:\"%s\"\r\n", vendorID);
	    }
	}
	else
	{
	}

#if 0	
    printf("\r\nkiiDemo_pushMessageCallback\r\n");    
/*	
	printf("\r\n");
    for (i=0; i<rcvdCounter; i++)
    {
        printf("%02x", jsonBuf[i]);
    }
*/
	printf("\r\n=========================\r\n");
       printf("%s", jsonBuf);
	printf("\r\n=========================\r\n");
    if (kiiObj_create(STR_BUCKET_TO_SERVER, STR_JSONOBJECT, STR_MEDIA_TYPE, objectID) < 0)
    {
        printf("kii create object with data type failed !\r\n");
    }
    else
    {
        printf("kii object is created with data type, objectID:\"%s\"\r\n", objectID);
    }

#endif	
}


void kiiDemo_testPush(void)
{
    if (kiiPush_createTopic(STR_THING_TOPIC) < 0)
    {
 	printf("create bucket ""%s""error !\r\n", STR_THING_TOPIC);
    }

    if (kiiPush_subscribeTopic(STR_THING_TOPIC) < 0)
    {
 	printf("subscribe bucket ""%s""error !\r\n", STR_THING_TOPIC);
    }

    if (kiiPush_subscribeAppBucket(STR_BUCKET_FIRWAREUPGRADE) < 0)
    {
 	printf("Subscribe bucket ""%s""error !\r\n", STR_BUCKET_FIRWAREUPGRADE);
    }

    if (kiiPush_subscribeThingBucket(STR_BUCKET_FROM_SERVER) < 0)
    {
 	printf("Subscribe bucket ""%s""error !\r\n", STR_BUCKET_FROM_SERVER);
    }

	
    if (KiiPush_init(DEMO_KII_TASK_PRIO, kiiDemo_pushMessageCallback) < 0)
    {
	printf("Init push error !\r\n");
	return;
    }
    else
    {
	printf("Init push success !\r\n");
    }
}

int kiiDemo_OnBoarding(void)
{
    unsigned char mac_addr[8];
    char vendorID[KII_DEVICE_VENDOR_ID+1] ;
    int i;	

    printf("Device on boarding ...\r\n");
    memset(mac_addr,0,sizeof(mac_addr));
    tls_get_mac_addr(mac_addr);
    memset(vendorID, 0, sizeof(vendorID));
    for(i=0; i<6; i++)
    {
        sprintf(vendorID+strlen(vendorID), "%02x", mac_addr[i]);
    }
	
    //strcpy(vendorID+strlen(vendorID), "12");
	
    printf("verdorID:""%s""\r\n", vendorID);

	
    if (kiiDev_getToken(vendorID, PASSWORD) != 0)
    {
        printf("Get token failed, try to register\r\n");
        if (kiiDev_register(vendorID, DEVICE_TYPE, PASSWORD) != 0)
        {
	    return -1;
        }
	else
	{
	    return 0;
	}
    }
    else
    {
	    return 0;
    }
}

int kiiDemo_test(char *buf)
{
    printf("Kii demo test for firmware upgrade\r\n");
	
    if (kii_init(STR_SITE, STR_APPID, STR_APPKEY) < 0)
    {
    	    printf("kii init failed\r\n");
	    return WM_FAILED;
    }
    if (kiiDemo_OnBoarding() != 0)
    {
    	    printf("device on bording failed\r\n");
	    return WM_FAILED;
    }

    //kiiDemo_testObject();
    kiiDemo_testPush();
    return WM_SUCCESS;

}


