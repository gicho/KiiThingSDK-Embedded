package com.youwill.store.activities;

import com.kii.cloud.storage.Kii;
import com.kii.cloud.storage.social.KiiSocialConnect;
import com.kii.cloud.storage.social.connector.KiiSocialNetworkConnector;
import com.youwill.store.R;
import com.youwill.store.utils.DataUtils;
import com.youwill.store.utils.LogUtils;
import com.youwill.store.utils.Settings;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.text.TextUtils;
import android.webkit.CookieManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLDecoder;

/**
 * Created by tian on 14/10/22:下午10:51.
 */
public class LogIn2Activity extends Activity {

    private WebView webview;

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log_in);
        webview = (WebView) findViewById(R.id.webview);
        InnerWebViewClient client = new InnerWebViewClient();
        webview.setWebViewClient(client);
        WebSettings settings = webview.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setSaveFormData(false);
        settings.setCacheMode(WebSettings.LOAD_NO_CACHE);

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.removeAllCookie();
        refresh();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == KiiSocialNetworkConnector.REQUEST_CODE && resultCode == RESULT_OK) {
            Kii.socialConnect(KiiSocialConnect.SocialNetwork.SOCIALNETWORK_CONNECTOR)
                    .respondAuthOnActivityResult(
                            requestCode,
                            resultCode,
                            data);
        }
        finish();
    }

    private boolean shouldFinish(String url) {
        if (url.contains("kii_access_token")) {
            Bundle args = decodeUrl(url);
            String accessToken = args.getString("kii_access_token");
            String uid = args.getString("kii_user_id");
            Settings.setToken(this, accessToken);
            Settings.setUserId(this, uid);
            return true;
        } else {
            return false;
        }
    }

    public static Bundle decodeUrl(String s) {
        Bundle params = new Bundle();
        if (s != null) {
            try {
                URL u = new URL(s);
                String queryParams = u.getQuery();
                String array[] = queryParams.split("&");
                for (String parameter : array) {
                    if (TextUtils.isEmpty(parameter)) {
                        continue;
                    }
                    String v[] = parameter.split("=");
                    if (v.length < 2) {
                        continue;
                    }
                    params.putString(URLDecoder.decode(v[0], "UTF-8"),
                            URLDecoder.decode(v[1], "UTF-8"));
                }
            } catch (MalformedURLException e) {
                e.printStackTrace();
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }
        return params;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        webview.clearCache(true);
    }

    public void refresh() {
        webview.stopLoading();
        webview.loadUrl("about:blank");
        webview.loadUrl(OAUTH_URL);
    }

    public static final String OAUTH_URL
            = "http://api-cn2.kii.com/api/apps/c99e04f1/integration/webauth/connect?id=youwill";

    private class InnerWebViewClient extends WebViewClient {

        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            LogUtils.d("shouldOverrideUrlLoading? " + url);
            view.loadUrl(url);
            return true;
        }

        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon) {
            LogUtils.d("onPageStarted, url is " + url);
            if (shouldFinish(url)) {
                new Thread(){
                    @Override
                    public void run() {
                        DataUtils.getPurchasedList(getApplicationContext());
                    }
                }.start();
                Toast.makeText(LogIn2Activity.this, R.string.log_in_success, Toast.LENGTH_SHORT)
                        .show();
                setResult(RESULT_OK);
                finish();
            } else {
                super.onPageStarted(view, url, favicon);
            }
        }
    }

}