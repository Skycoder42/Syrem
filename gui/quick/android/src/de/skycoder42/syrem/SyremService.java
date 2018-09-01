package de.skycoder42.syrem;

import android.content.Intent;

import android.os.Bundle;

import android.graphics.BitmapFactory;

import android.support.v4.app.NotificationCompat;
import android.support.v4.app.RemoteInput;

import org.qtproject.qt5.android.bindings.QtService;

public class SyremService extends QtService {
	@Override
	public void onDestroy() {
		super.onDestroy();
		// explicitly exit to prevent the process from beeing cached
		System.exit(0);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		int result = super.onStartCommand(intent, flags, startId);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, Globals.ForegroundChannelId)
			.setContentTitle(getString(R.string.svc_name))
			.setContentText(getString(R.string.svc_desc))
			.setContentInfo(getString(R.string.app_name))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(Globals.ForegroundColor)
			.setLocalOnly(true)
			.setOngoing(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		if (!Globals.isOreo())
			builder.setPriority(NotificationCompat.PRIORITY_MIN);

		startForeground(Globals.ForegroundId, builder.build());
		if(intent != null)
			handleIntent(intent);

		return result;
	}

	public void completeAction() {
		stopForeground(true);
		stopService(new Intent(this, SyremService.class));//Stop myself
	}

	private static native void handleIntent(String action, String remId, int versionCode, String resultExtra);

	private void handleIntent(Intent intent) {
		String remId = intent.getStringExtra(Globals.ExtraId);
		int versionCode = intent.getIntExtra(Globals.ExtraVersion, 0);

		// cancel complete notifications early for a smoother experience
		if(intent.getAction() == Globals.Actions.ActionComplete.getAction())
			Notifier.cancelExplicitly(this, remId);

		String resultExtra = null;
		if(intent.getAction() == Globals.Actions.ActionSnooze.getAction()) {
			Bundle remoteInput = RemoteInput.getResultsFromIntent(intent);
			if(remoteInput != null)
				resultExtra = remoteInput.getCharSequence(Globals.ExtraSnoozeTime).toString();
		}

		handleIntent(intent.getAction(), remId, versionCode, resultExtra);
	}
}
