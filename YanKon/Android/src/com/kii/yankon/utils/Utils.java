package com.kii.yankon.utils;

import android.content.ContentValues;
import android.content.Context;
import android.content.res.Resources;
import android.database.Cursor;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.widget.Toast;

import com.kii.cloud.storage.KiiUser;
import com.kii.yankon.model.Light;
import com.kii.yankon.providers.YanKonProvider;
import com.kii.yankon.services.NetworkSenderService;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

/**
 * Created by tian on 14/11/25:上午9:00.
 */
public class Utils {
    public static float dp2Px(Context context, float dp) {
        Resources resources = context.getResources();
        DisplayMetrics metrics = resources.getDisplayMetrics();
        float px = dp * (metrics.densityDpi / 160f);
        return px;
    }

    public static int px2Dp(Context context, int pixel) {
        int dp = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_PX, pixel,
                context.getResources().getDisplayMetrics());
        return dp;
    }

    public static String byteArrayToString(byte[] cmd, char seperator) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < cmd.length; i++) {
            if (seperator != 0)
                sb.append(seperator);
            sb.append(Utils.ByteToHexString(cmd[i]));
        }
        return sb.toString();
    }

    public static String byteArrayToString(byte[] cmd) {
        return byteArrayToString(cmd, ' ');
    }

    public static void byteArrayToString(byte[] cmd, StringBuilder sb) {
        for (int i = 0; i < cmd.length; i++) {
            sb.append(' ');
            sb.append(Utils.ByteToHexString(cmd[i]));
        }
    }

    public static int unsignedByteToInt(byte b) {
        return (int) b & 0xFF;
    }

    public static int readInt16(byte[] data, int offset) {
        return unsignedByteToInt(data[offset + 1]) * 256 + unsignedByteToInt(data[offset]);
    }

    public static void Int16ToByte(int data, byte[] arr, int pos) {
        arr[pos + 1] = (byte) (data / 256);
        arr[pos] = (byte) (data % 256);
    }

    public static int getRGBColor(byte[] data) {
        return unsignedByteToInt(data[2]) * 256 * 256 + unsignedByteToInt(data[1]) * 256 + unsignedByteToInt(data[0]);
    }

    public static String stringFromBytes(byte[] data) {
        int size = 0;
        while (size < data.length) {
            if (data[size] == 0) {
                break;
            }
            size++;
        }
        return new String(data, 0, size);
    }

    public static int char2int(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }
        return 0;
    }

    public static String ByteToHexString(byte b) {
        char[] ch = new char[2];
        int a = unsignedByteToInt(b);
        int t = a / 16;
        if (t < 10) {
            ch[0] = (char) ('0' + t);
        } else {
            ch[0] = (char) ('A' + t - 10);
        }

        t = a % 16;
        if (t < 10) {
            ch[1] = (char) ('0' + t);
        } else {
            ch[1] = (char) ('A' + t - 10);
        }
        return new String(ch);
    }

    public static void controlLight(final Context context, final int light_id, boolean doItNow) {
        final Light light;
        Cursor c = context.getContentResolver().query(YanKonProvider.URI_LIGHTS, null, "_id=" + light_id, null, null);
        if (c != null) {
            if (c.moveToFirst()) {
                light = new Light();
                light.brightness = c.getInt(c.getColumnIndex("brightness"));
                light.CT = c.getInt(c.getColumnIndex("CT"));
                light.color = c.getInt(c.getColumnIndex("color"));
                light.ip = c.getString(c.getColumnIndex("IP"));
                light.mac = c.getString(c.getColumnIndex("MAC"));
                light.state = c.getInt(c.getColumnIndex("state")) != 0;
                light.connected = c.getInt(c.getColumnIndex("connected")) != 0;
            } else {
                light = null;
            }
            c.close();
        } else {
            light = null;
        }
        if (light == null) {
            Toast.makeText(context, "Cannot load light info", Toast.LENGTH_SHORT).show();
            return;
        }
        if (light.connected) {
            if (TextUtils.isEmpty(light.ip)) {
                Toast.makeText(context, "Cannot get light IP", Toast.LENGTH_SHORT).show();
            } else {
                byte[] cmd = CommandBuilder.buildLightInfo(1, light.state, light.color, light.brightness, light.CT);
                NetworkSenderService.sendCmd(context, light.ip, cmd);
            }
        } else if (doItNow && KiiUser.isLoggedIn()) {
            new Thread() {
                @Override
                public void run() {
                    JSONObject lights = new JSONObject();
                    try {
                        lights.put(light.mac, light.remotePassword);
                    } catch (JSONException e) {
                    }
                    KiiSync.fireLamp(lights, false, light.state ? 1 : 0, light.color, light.brightness, light.CT);
                }
            }.start();
        }
    }

    public static void controlGroup(final Context context, final int group_id, boolean doItNow) {
        int brightness = Constants.DEFAULT_BRIGHTNESS;
        int CT = Constants.DEFAULT_CT;
        int color = Constants.DEFAULT_COLOR;
        boolean state = true;
        boolean loadedInfo = false;
        Cursor c = context.getContentResolver().query(YanKonProvider.URI_LIGHT_GROUPS, null, "_id=" + group_id, null, null);
        if (c != null) {
            if (c.moveToFirst()) {
                brightness = c.getInt(c.getColumnIndex("brightness"));
                CT = c.getInt(c.getColumnIndex("CT"));
                color = c.getInt(c.getColumnIndex("color"));
                state = c.getInt(c.getColumnIndex("state")) != 0;
                loadedInfo = true;
            }
            c.close();
        }
        if (!loadedInfo) {
            Toast.makeText(context, "Cannot load group info", Toast.LENGTH_SHORT).show();
            return;
        }

        ArrayList<String> connectedLights = new ArrayList<>();
        final JSONObject unconnectedLights = new JSONObject();
        StringBuilder allIds = new StringBuilder();
        c = context.getContentResolver().query(YanKonProvider.URI_LIGHT_GROUP_REL, null, "group_id=" + group_id, null, null);
        if (c != null) {
            while (c.moveToNext()) {
                boolean connected = c.getInt(c.getColumnIndex("connected")) > 0;
                String ip = c.getString(c.getColumnIndex("IP"));
                String mac = c.getString(c.getColumnIndex("MAC"));
                String remotePwd = c.getString(c.getColumnIndex("remote_pwd"));
                int _id = c.getInt(c.getColumnIndex("_id"));
                if (allIds.length() > 0)
                    allIds.append(',');
                allIds.append(_id);
                if (connected && !TextUtils.isEmpty(ip)) {
                    connectedLights.add(ip);
                } else {
                    try {
                        if (remotePwd == null)
                            remotePwd = "";
                        unconnectedLights.put(mac, remotePwd);
                    } catch (JSONException e) {
                    }
                }
            }
            c.close();
        }
        if (allIds.length() > 0) {
            ContentValues values = new ContentValues();
            values.put("CT", CT);
            values.put("brightness", brightness);
            values.put("state", state);
            values.put("color", color);
            context.getContentResolver().update(YanKonProvider.URI_LIGHTS, values, "_id in (" + allIds.toString() + ")", null);
        }
        if (connectedLights.size() > 0) {
            byte[] cmd = CommandBuilder.buildLightInfo(1, state, color, brightness, CT);
            String[] ips = connectedLights.toArray(new String[0]);
            NetworkSenderService.sendCmd(context, ips, cmd);
        }
        if (doItNow && KiiUser.isLoggedIn()) {
            final boolean i_state = state;
            final int i_color = color;
            final int i_brightness = brightness;
            final int i_CT = CT;
            new Thread() {
                @Override
                public void run() {
                    KiiSync.fireLamp(unconnectedLights, false, i_state ? 1 : 0, i_color, i_brightness, i_CT);
                }
            }.start();
        }
    }
}
