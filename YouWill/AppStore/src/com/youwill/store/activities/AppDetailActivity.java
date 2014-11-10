package com.youwill.store.activities;

import com.nostra13.universalimageloader.core.ImageLoader;
import com.youwill.store.R;
import com.youwill.store.providers.YouWill;
import com.youwill.store.utils.AppUtils;
import com.youwill.store.utils.Utils;

import org.json.JSONArray;
import org.json.JSONObject;

import android.app.Activity;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.text.format.DateFormat;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RatingBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class AppDetailActivity extends Activity implements View.OnClickListener {

    public static final String EXTRA_APP_ID = "appId";

    String mAppId;

    JSONObject mAppInfo;

    RecyclerView mRecyclerView;

    LinearLayoutManager mLinearLayoutManager;

    private List<String> mPics = new ArrayList<String>();

    private Button mPriceBtn;

    private ProgressBar mProgressBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_app_detail);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_DIM_BEHIND,
                WindowManager.LayoutParams.FLAG_DIM_BEHIND);
        getWindow().setLayout(585, 556);
        mAppId = getIntent().getStringExtra(EXTRA_APP_ID);
        if (TextUtils.isEmpty(mAppId)) {
            finish();
            return;
        }

        Cursor c = getContentResolver().query(YouWill.Application.CONTENT_URI,
                null,
                YouWill.Application.APP_ID + "=(?)",
                new String[]{mAppId},
                null);
        if (c != null && c.moveToFirst()) {
            String app_info_str = c.getString(c.getColumnIndex(YouWill.Application.APP_INFO));
            try {
                mAppInfo = new JSONObject(app_info_str);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        Utils.closeSilently(c);

        if (mAppInfo == null) {
            finish();
            return;
        }
        initViews();
    }

    protected void initViews() {
        ImageView closeView = (ImageView) findViewById(R.id.close);
        closeView.setOnClickListener(this);
        ImageView iconView = (ImageView) findViewById(R.id.app_detail_icon);
        String iconUrl = mAppInfo.optString("icon");
        ImageLoader.getInstance().displayImage(iconUrl, iconView, Utils.iconDisplayOptions);
        TextView tv = (TextView) findViewById(R.id.app_detail_name);
        tv.setText(mAppInfo.optString("name"));
        tv = (TextView) findViewById(R.id.app_detail_developer);
        tv.setText(mAppInfo.optString("developer_name"));
        StringBuilder info = new StringBuilder();
        tv = (TextView) findViewById(R.id.app_detail_info);
        int size = mAppInfo.optInt("apk_size");
        info.append(Utils.getFileSizeString(size)).append(" ");
        int downloadCount = mAppInfo.optInt("download_count");
        String downloadCountString = downloadCount + getString(R.string.download_count);
        info.append(downloadCountString).append(" ");
        info.append(getString(R.string.version_prompt)).append(mAppInfo
                .optString("version_name")).append(" ");
        long updateTime = mAppInfo.optLong("_modified", -1);
        if (updateTime < 0) {
            updateTime = mAppInfo.optLong("_created");
        }
        String updateTimeString = getString(R.string.update_time) + DateFormat
                .format("YYYY-MM-dd", updateTime);
        info.append(updateTimeString);
        tv.setText(info.toString());
        tv = (TextView) findViewById(R.id.app_detail_desc);
        tv.setText(mAppInfo.optString("description"));
        tv = (TextView) findViewById(R.id.app_detail_upgrade);
        tv.setText(mAppInfo.optString("what_is_new", getString(R.string.none)));
        JSONArray thumbnails = mAppInfo.optJSONArray("thumbnails");
        if (thumbnails != null && thumbnails.length() > 0) {
            for (int i = 0; i < thumbnails.length(); i++) {
                mPics.add(thumbnails.optString(i));
            }
        }
        RatingBar ratingBar = (RatingBar) findViewById(R.id.app_detail_rate);
        ratingBar.setRating((float) mAppInfo.optDouble("rating_score"));
        mPriceBtn = (Button) findViewById(R.id.app_detail_price);
        AppUtils.bindButton(this, mAppInfo, mPriceBtn);
        mProgressBar = (ProgressBar) findViewById(R.id.app_detail_progress);
        AppUtils.bindProgress(mAppId, mProgressBar, Utils.getStatus(mAppInfo));
        mPriceBtn.setOnClickListener(this);
        mRecyclerView = (RecyclerView) findViewById(R.id.pic_layout);
        mLinearLayoutManager = new LinearLayoutManager(this);
        mLinearLayoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);
        mRecyclerView.setLayoutManager(mLinearLayoutManager);
        PicAdapter adapter = new PicAdapter();
        mRecyclerView.setAdapter(adapter);
        if (adapter.getItemCount() > 0) {
            mRecyclerView.setVisibility(View.VISIBLE);
            mRecyclerView.scrollToPosition(10000);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.app_detail_price:
                AppUtils.clickPriceButton(this, mAppInfo);
                AppUtils.bindButton(this, mAppInfo, mPriceBtn);
                AppUtils.bindProgress(mAppId, mProgressBar, Utils.getStatus(mAppInfo));
                mHandler.sendEmptyMessageDelayed(MSG_UPDATE_PROGRESS, DELAY_TIME);
                break;
            case R.id.close:
                finish();
                break;
        }
    }

    private static final int MSG_UPDATE_PROGRESS = 0;

    private static final int DELAY_TIME = 1000;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_PROGRESS:
                    AppUtils.bindProgress(mAppId, mProgressBar, Utils.getStatus(mAppInfo));
                    AppUtils.bindButton(AppDetailActivity.this, mAppInfo, mPriceBtn);
                    sendEmptyMessageDelayed(MSG_UPDATE_PROGRESS, DELAY_TIME);
                    break;
            }
        }
    };

    @Override
    protected void onDestroy() {
        try {
            mHandler.removeMessages(MSG_UPDATE_PROGRESS);
            mHandler = null;
        } catch (Exception e) {

        }
        super.onDestroy();
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {

        private ImageView mImageView;

        public ViewHolder(View itemView) {
            super(itemView);
            mImageView = (ImageView) itemView;
        }

    }

    private class PicAdapter extends RecyclerView.Adapter<ViewHolder> {

        @Override
        public ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i) {
            View v = getLayoutInflater().inflate(R.layout.app_detail_pic_item_portrait, viewGroup,
                    false);
            return new ViewHolder(v);
        }

        @Override
        public void onBindViewHolder(ViewHolder viewHolder, int i) {
            ImageLoader.getInstance()
                    .displayImage(mPics.get(i % mPics.size()), viewHolder.mImageView);
        }

        @Override
        public int getItemCount() {
            return Integer.MAX_VALUE;
        }
    }
}