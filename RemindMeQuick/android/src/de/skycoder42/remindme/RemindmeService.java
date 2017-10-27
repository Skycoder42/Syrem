package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import org.qtproject.qt5.android.bindings.QtService;

public class RemindmeService extends QtService {
	public static void startService(Context context) {
		context.startService(new Intent(context, RemindmeService.class));
	}
}
