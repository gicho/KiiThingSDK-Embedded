package com.kii.yankon.utils;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.util.Log;

import com.kii.cloud.storage.Kii;
import com.kii.cloud.storage.KiiBucket;
import com.kii.cloud.storage.KiiObject;
import com.kii.cloud.storage.KiiServerCodeEntry;
import com.kii.cloud.storage.KiiServerCodeEntryArgument;
import com.kii.cloud.storage.KiiServerCodeExecResult;
import com.kii.cloud.storage.KiiUser;
import com.kii.yankon.providers.YanKonProvider;

import org.json.JSONObject;

/**
 * Created by Evan on 14/12/20.
 */
public class KiiSync {

    private static final String LOG_TAG = "KiiSync";

    public static boolean syncLights(Context context, Cursor cursor) {
        KiiUser kiiUser = KiiUser.getCurrentUser();
        if (kiiUser == null) {
            return false;
        }
        boolean syncResult = true;
        KiiBucket bucket = kiiUser.bucket("lights");
        if (cursor.moveToFirst()) {
            do {
                boolean synced = cursor.getInt(cursor.getColumnIndex("synced")) > 0;
                if (synced) {
                    continue;
                }
                int light_id = cursor.getInt(cursor.getColumnIndex("_id"));
                KiiObject lightObj;
                String mac = cursor.getString(cursor.getColumnIndex("MAC"));
                lightObj = bucket.object(mac);
                lightObj.set("name", cursor.getString(cursor.getColumnIndex("name")));
                lightObj.set("MAC", mac);
                lightObj.set("light_id", light_id);
                lightObj.set("brightness", cursor.getInt(cursor.getColumnIndex("brightness")));
                lightObj.set("CT", cursor.getInt(cursor.getColumnIndex("CT")));
                lightObj.set("color", cursor.getInt(cursor.getColumnIndex("color")));
                lightObj.set("state", cursor.getInt(cursor.getColumnIndex("state")) > 0);
                lightObj.set("owned_time", cursor.getLong(cursor.getColumnIndex("owned_time")));
                try {
                    lightObj.saveAllFields(true);
                    ContentValues values = new ContentValues();
                    values.put("synced", true);
                    context.getContentResolver().update(YanKonProvider.URI_LIGHTS, values, "_id=" + light_id, null);
                } catch (Exception e) {
                    syncResult = false;
                    Log.e(LOG_TAG, Log.getStackTraceString(e));
                }
            } while (cursor.moveToNext());
        }
        return syncResult;
    }

    public static boolean syncLightGroups(Context context, Cursor cursor) {
        KiiUser kiiUser = KiiUser.getCurrentUser();
        if (kiiUser == null) {
            return false;
        }
        boolean syncResult = true;
        KiiBucket bucket = kiiUser.bucket("light_groups");
        if (cursor.moveToFirst()) {
            do {
                boolean synced = cursor.getInt(cursor.getColumnIndex("synced")) > 0;
                if (synced) {
                    continue;
                }
                int group_id = cursor.getInt(cursor.getColumnIndex("_id"));
                KiiObject groupObj;
                String objectID = "id" + group_id;
                groupObj = bucket.object(objectID);
                groupObj.set("name", cursor.getString(cursor.getColumnIndex("name")));
                groupObj.set("group_id", group_id);
                groupObj.set("brightness", cursor.getInt(cursor.getColumnIndex("brightness")));
                groupObj.set("CT", cursor.getInt(cursor.getColumnIndex("CT")));
                groupObj.set("color", cursor.getInt(cursor.getColumnIndex("color")));
                groupObj.set("state", cursor.getInt(cursor.getColumnIndex("state")) > 0);
                groupObj.set("owned_time", cursor.getLong(cursor.getColumnIndex("owned_time")));
                try {
                    groupObj.saveAllFields(true);
                    ContentValues values = new ContentValues();
                    values.put("synced", true);
                    context.getContentResolver().update(YanKonProvider.URI_LIGHT_GROUPS, values, "_id=" + group_id, null);
                } catch (Exception e) {
                    syncResult = false;
                    Log.e(LOG_TAG, Log.getStackTraceString(e));
                }
            } while (cursor.moveToNext());
        }
        return syncResult;
    }

    public static String registLamp(String MAC) {
        String result = null;
        KiiServerCodeEntry entry = Kii.serverCodeEntry("registLamp");

        try {
            JSONObject rawArg = new JSONObject();

            rawArg.put("thingID", MAC);
            rawArg.put("batchName", 100);
            KiiServerCodeEntryArgument arg = KiiServerCodeEntryArgument
                    .newArgument(rawArg);

            // Execute the Server Code
            KiiServerCodeExecResult res = entry.execute(arg);

            // Parse the result.
            JSONObject returned = res.getReturnedValue();
            result = returned.getString("returnedValue");
        } catch (Exception e) {

        }
        return result;
    }

    public static String fireLamp(String MAC, int state, int color, int brightness, int CT) {
        String result = null;
        KiiServerCodeEntry entry = Kii.serverCodeEntry("fireLamp");

        try {
            JSONObject action =  new JSONObject();
            action.put("state",state);
            action.put("color",color);
            action.put("brightness",brightness);
            action.put("CT",CT);
            JSONObject rawArg = new JSONObject();
            rawArg.put("thingID", MAC);
            rawArg.put("action", action);
            KiiServerCodeEntryArgument arg = KiiServerCodeEntryArgument
                    .newArgument(rawArg);

            // Execute the Server Code
            KiiServerCodeExecResult res = entry.execute(arg);

            // Parse the result.
            JSONObject returned = res.getReturnedValue();
            result = returned.getString("returnedValue");
        } catch (Exception e) {

        }
        return result;
    }
}
