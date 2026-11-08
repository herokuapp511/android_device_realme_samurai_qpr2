/*
 * SPDX-FileCopyrightText: 2026
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.settings.chargespeed;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.UserHandle;
import android.util.Log;

/**
 * Remote preference updater for LineageParts charging control screen.
 * Feeds dynamic summary (selected speed option) to RemotePreference.
 */
public class ChargingSpeedUpdater extends BroadcastReceiver {
    private static final String TAG = "ChargingSpeedUpdater";

    public static final String ACTION_REFRESH_PREFERENCE =
            "lineageos.intent.action.REFRESH_PREFERENCE";
    public static final String ACTION_UPDATE_PREFERENCE =
            "lineageos.intent.action.UPDATE_PREFERENCE";

    public static final String EXTRA_KEY = ":lineage:pref_key";
    public static final String EXTRA_SUMMARY = ":lineage:pref_summary";
    public static final String PERMISSION_MANAGE_REMOTE_PREFERENCES =
            "lineageos.permission.MANAGE_REMOTE_PREFERENCES";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) {
            return;
        }
        if (isOrderedBroadcast() && ACTION_UPDATE_PREFERENCE.equals(intent.getAction())) {
            final String key = intent.getStringExtra(EXTRA_KEY);
            if (key != null) {
                Bundle extras = getResultExtras(true);
                extras.putString(EXTRA_KEY, key);
                extras.putString(EXTRA_SUMMARY, CoolDownUtils.getSummary(context));
                setResultCode(Activity.RESULT_OK);
                return;
            }
            abortBroadcast();
        }
    }

    public static void notifyChanged(Context context, String key) {
        Intent intent = new Intent(ACTION_REFRESH_PREFERENCE);
        intent.putExtra(EXTRA_KEY, key);
        try {
            context.sendBroadcastAsUser(intent, UserHandle.CURRENT, PERMISSION_MANAGE_REMOTE_PREFERENCES);
        } catch (Exception e) {
            Log.w(TAG, "sendBroadcastAsUser failed, falling back to sendBroadcast", e);
            try {
                context.sendBroadcast(intent, PERMISSION_MANAGE_REMOTE_PREFERENCES);
            } catch (Exception ex) {
                Log.w(TAG, "sendBroadcast failed", ex);
            }
        }
    }
}
