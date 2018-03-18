import QtQml 2.2
import QtQuick 2.10
import QtQuick.Controls 2.3

SpinBox {
	id: timeEdit

	property var inputValue: new Date()
	property var minimum: new Date(0, 0, 0, 0, 0)
	property var maximum: new Date(0, 0, 0, 23, 59)
	property var timeStep: new Date(0, 0, 0, 00, 05)

	editable: true

	from: dateToInt(minimum)
	to: dateToInt(maximum)
	stepSize: dateToInt(timeStep)
	value: dateToInt(inputValue)
	onValueModified: inputValue = intToDate(value)

	validator: RegExpValidator {
		regExp: {
			var locale = Qt.locale();
			var regStr = locale.timeFormat(Locale.ShortFormat)
				.replace("hh", "\\d{2}")
				.replace("h", "\\d{1,2}")
				.replace("HH", "\\d{2}")
				.replace("H", "\\d{1,2}")
				.replace("mm", "\\d{2}")
				.replace("m", "\\d{1,2}")
				.replace("ss", "\\d{2}")
				.replace("s", "\\d{1,2}")
				.replace("zzz", "\\d{3}")
				.replace("z", "\\d{1,3}")
				.replace("AP", locale.amText.toUpperCase())
				.replace("A", locale.amText.toUpperCase())
				.replace("ap", locale.amText.toLowerCase())
				.replace("a", locale.amText.toLowerCase());
			return new RegExp(regStr, "i");
		}
	}

	textFromValue: function(value, locale) {
		return intToDate(value).toLocaleTimeString(locale, Locale.ShortFormat);
	}

	valueFromText: function(text, locale) {
		var date = Date.fromLocaleTimeString(locale, text, Locale.ShortFormat);
		if(date)
			return dateToInt(date);
		else
			return value;
	}

	function intToDate(value) {
		return new Date(0, 0, 0, Math.floor(value / 60), value % 60);
	}

	function dateToInt(date) {
		if (typeof date === "string") {
			var splitStr = date.split(":")
			date = new Date(0, 0, 0, splitStr[0], splitStr[1])
		}
		return date.getHours() * 60 + date.getMinutes();
	}
}
