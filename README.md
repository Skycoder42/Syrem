# Syrem - Synchronized Reminders
A simple reminder application for desktop and mobile, with synchronized reminders

[![Travis Build Status](https://travis-ci.org/Skycoder42/Syrem.svg?branch=master)](https://travis-ci.org/Skycoder42/Syrem)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/t0d9gc1muekbdv97?svg=true)](https://ci.appveyor.com/project/Skycoder42/syrem)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/721ecc2c4dcc443dbed4d8a2dec4dd8f)](https://www.codacy.com/app/Skycoder42/Syrem)
[![AUR](https://img.shields.io/aur/version/syrem.svg)](https://aur.archlinux.org/packages/syrem/)
[![Chocolatey](https://img.shields.io/chocolatey/v/syrem.svg)](https://chocolatey.org/packages/syrem/)

<a href='https://play.google.com/store/apps/details?id=de.skycoder42.syrem'><img alt='Get it on Google Play' src='https://play.google.com/intl/en_us/badges/images/generic/en_badge_web_generic.png' height="80px"/></a>

## Features
- Create reminders from natural expressions like "in 3 weeks on Monday"
	- supports repeated reminders
	- works with timepoints and timespans
	- make reminders important for persistant notifications
- Allows you to synchronize your reminders between different platforms
	- Currently Linux, Android, Windows and MacOs are supported
	- Available for any linux distro via [flatpak](https://flatpak.org/). See installation below
- Native notification support on linux and android

## Screenshots
### Desktop
| Overview | Add Reminders
|----------|---------------
| ![desktop overview](screenshots/desktop_main.png) | ![desktop add](screenshots/desktop_add.png)
| Snooze Reminders | Notifications (on KDE Desktop)
| ![desktop snooze](screenshots/desktop_snooze.png) | ![desktop add](screenshots/desktop_notify.png)

### Mobile
| Overview | Add Reminders | Snooze Reminders
|----------|---------------|------------------
| ![desktop overview](screenshots/mobile_main.webp) | ![desktop add](screenshots/mobile_add.webp) | ![desktop snooze](screenshots/mobile_snooze.webp)
| | Notifications |
| | ![desktop add](screenshots/mobile_notify.webp) |

## Installation
1. **Arch Linux:** Use the AUR-Package: [`syrem`](https://aur.archlinux.org/packages/syrem/)
2. **Other Linux distributions:** Install via [flatpak](https://flatpak.org/): `sudo flatpak install 'https://raw.githubusercontent.com/Skycoder42/Syrem/master/flatpak/de.skycoder42.syrem.flatpakref'`
	- Flatpak packages work on any linux distribution, as long as you have flatpak installed. See https://flatpak.org/setup/ to get flatpak for your distribution
	- Alternative install method:
		1. Add The repository: `sudo flatpak repo-add skycoder42 'https://raw.githubusercontent.com/Skycoder42/Syrem/master/flatpak/de.skycoder42.flatpakrepo'`
		2. Install: `sudo flatpak install skycoder42 de.skycoder42.syrem`
3. **Android:** While the apk is available as direct download, you can also get it via google play store: [Syrem Google Play](https://play.google.com/store/apps/details?id=de.skycoder42.syrem)
4. **Windows:** Get it via [chocolatey](https://chocolatey.org/): `choco install syrem`
	- See [Syrem - Chocolatey](https://chocolatey.org/packages/syrem/) for package details
	- Chocolatey can be install using the following description: https://chocolatey.org/install#installing-chocolatey
	- You can also install the chocolatey GUI after installing chocolatey itself: `choco install chocolateygui`
5. **MacOs:** Provided via [homebrew cask](https://github.com/Homebrew/homebrew-cask):
	1. First add the tap: `brew tap Skycoder42/qt-modules`
	2. Then install the package: `brew cask install syrem`

## Attributions
### Icon Sources
- Icons from <a href="https://www.flaticon.com/authors/roundicons" title="Roundicons">Roundicons</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> are licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
- http://www.fatcow.com/free-icons
- https://material.io/icons/

### Android libraries
- https://github.com/leolin310148/ShortcutBadger
