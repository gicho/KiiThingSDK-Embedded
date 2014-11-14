package com.youwill.store;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.SparseArray;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import com.youwill.store.activities.LogInActivity;
import com.youwill.store.activities.SettingsActivity;
import com.youwill.store.fragments.*;
import com.youwill.store.utils.DataUtils;
import com.youwill.store.utils.Settings;
import com.youwill.store.utils.Utils;

public class MainActivity extends Activity implements View.OnClickListener {

    private static final String TAG = MainActivity.class.getSimpleName();

    private String[] tabs = new String[4];

    private int currentFragmentIndex;

    private Fragment currentFragment;

    private SparseArray<Fragment> fragments = new SparseArray<Fragment>(4);

    private EditText searchEdit;

    private SearchFragment mSearchFragment;

    private TextView mLogInView;

    private ImageView mSettingsView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tabs[0] = getString(R.string.hot_sale);
        tabs[1] = getString(R.string.categories);
        tabs[2] = getString(R.string.purchased);
        tabs[3] = getString(R.string.upgrade);
        findViewById(R.id.hot_button).setOnClickListener(this);
        findViewById(R.id.categories_button).setOnClickListener(this);
        findViewById(R.id.upgrade_button).setOnClickListener(this);
        findViewById(R.id.purchased_button).setOnClickListener(this);
        mLogInView = (TextView) findViewById(R.id.log_in_text);
        setLogInText();
        mSettingsView = (ImageView) findViewById(R.id.settings_button);
        mLogInView.setOnClickListener(this);
        mSettingsView.setOnClickListener(this);
        mSearchFragment = (SearchFragment) Fragment
                .instantiate(this, SearchFragment.class.getName());
        if (savedInstanceState == null) {
            fragments.put(R.id.hot_button,
                    Fragment.instantiate(this, HotFragment.class.getName()));
            fragments.put(R.id.categories_button,
                    Fragment.instantiate(this, CategoriesFragment.class.getName()));
            fragments.put(R.id.upgrade_button,
                    Fragment.instantiate(this, UpgradeFragment.class.getName()));
            fragments.put(R.id.purchased_button,
                    Fragment.instantiate(this, PurchasedFragment.class.getName()));
            currentFragment = fragments.get(R.id.hot_button);
            currentFragmentIndex = R.id.hot_button;
            getFragmentManager().beginTransaction()
                    .add(R.id.fragments, currentFragment, String.valueOf(currentFragmentIndex))
                    .commit();
        }
        DataUtils.loadApps(this);
        initHeader();

        Settings.registerListener(this, mListener);
    }

    @Override
    protected void onDestroy() {
        Settings.unregisterListener(this, mListener);
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!Utils.isNetworkAvailable(this)) {
            new AlertDialog.Builder(this).setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.network_unavailable)
                    .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    }).setCancelable(false).show();
        }
    }

    private void initHeader() {
        View header = findViewById(R.id.main_header);
        searchEdit = (EditText) header.findViewById(R.id.search_edit);
        View searchButton = header.findViewById(R.id.search_button);
        View deleteButton = header.findViewById(R.id.delete_button);
        searchButton.setOnClickListener(this);
        deleteButton.setOnClickListener(this);
        deleteButton.requestFocus();
        searchEdit.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus && !mSearchFragment.isAdded()) {
                    showSearchFragment();
                }
            }
        });
        searchEdit.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (s != null && mSearchFragment.isAdded()) {
                    mSearchFragment.beginSearch(s.toString());
                }
            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.categories_button:
                //fall through;
            case R.id.hot_button:
                //fall through;
            case R.id.purchased_button:
                //fall through;
            case R.id.upgrade_button:
                switchFragment(v);
                break;
            case R.id.delete_button:
                searchEdit.setText("");
                break;
            case R.id.settings_button:
                startActivity(new Intent(this, SettingsActivity.class));
                break;
            case R.id.log_in_text:
                if (!Settings.isLoggedIn(this)) {
                    startActivity(new Intent(this, LogInActivity.class));
                }
                break;
            default:
                break;
        }

    }

    private void showSearchFragment() {
        if (mSearchFragment.isAdded()) {
            mSearchFragment.beginSearch(searchEdit.getText().toString());
        } else {
            FragmentTransaction transaction = getFragmentManager().beginTransaction();
            transaction.add(R.id.fragments, mSearchFragment);
            transaction.addToBackStack(null);
            transaction.commit();
            searchEdit.post(new Runnable() {
                @Override
                public void run() {
                    mSearchFragment.beginSearch(searchEdit.getText().toString());
                }
            });
        }
    }

    @Override
    public void onBackPressed() {
        if (mSearchFragment.isAdded()) {
            getFragmentManager().popBackStack();
        } else {
            finish();
        }
    }

    private void switchFragment(View v) {
        if (mSearchFragment.isAdded()) {
            getFragmentManager().popBackStack();
        }
        if (currentFragmentIndex == v.getId()) {
            return;
        }

        Fragment fragment = fragments.get(v.getId());
        if (fragment.isAdded()) {
            getFragmentManager().beginTransaction().hide(currentFragment).show(fragment).commit();
        } else {
            getFragmentManager().beginTransaction().hide(currentFragment)
                    .add(R.id.fragments, fragment, String.valueOf(v.getId())).commit();
        }
        currentFragmentIndex = v.getId();
        currentFragment = fragment;
        DataUtils.loadApps(this);
    }

    private SharedPreferences.OnSharedPreferenceChangeListener mListener
            = new SharedPreferences.OnSharedPreferenceChangeListener() {
        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
            if (key.contentEquals(Settings.USER_ID_KEY)) {
                setLogInText();
            }

            if (key.contentEquals(Settings.TOKEN_KEY) && !TextUtils.isEmpty(Settings.getToken(MainActivity.this))) {
                new Thread() {
                    @Override
                    public void run() {
                        DataUtils.getPurchasedList(getApplicationContext());
                    }
                }.start();
            }
        }
    };

    public void setLogInText() {
        if (Settings.isLoggedIn(this)) {
            mLogInView.setText(Settings.getNick(this));
        } else {
            mLogInView.setText(R.string.log_in);
        }
    }
}
