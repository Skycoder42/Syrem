# Maintainer: Felix Barz <skycoder42.de@gmx.de>
pkgname=remind-me
pkgver=1.0.0
pkgrel=1
pkgdesc="A simple reminder application for desktop and mobile, with synchronized reminders"
arch=('i686' 'x86_64')
url="https://github.com/Skycoder42/RemindMe"
license=('BSD')
depends=('qt5-base' 'qt5-svg' 'qt5-datasync' 'qt5-remoteobjects' )
makedepends=('qt5-tools' 'git' 'qpmx-qpmsource')
optdepends=("knotifications: Adds support for KDE notifications, which are more advanced (recompile required)"
			"repkg: Automatically rebuild the package on dependency updates")
_pkgfqn=$pkgname-$pkgver
source=("$_pkgfqn::git+https://github.com/Skycoder42/RemindMe.git") #tag=$pkgver")
sha256sums=('SKIP')

prepare() {
  mkdir -p build

  ln -s "$HOME/Programming/QtProjects/__private"
  cd "$_pkgfqn"

  echo "CONFIG += widgets_only" >> .qmake.conf #build widgets gui only
}

build() {
  cd build

  qmake "../$_pkgfqn/"
  make qmake_all
  make
  #skip for now: make lrelease
}

package() {
  cd build
  make INSTALL_ROOT="$pkgdir" install

  cd "../$_pkgfqn"
  # gui
  install -D -m644 icon/remindme.svg "$pkgdir/usr/share/icons/hicolor/scalable/apps/$pkgname.svg"
  install -D -m644 icon/remindme_error.svg "$pkgdir/usr/share/icons/hicolor/scalable/apps/$pkgname-error.svg"
  install -D -m644 RemindMeWidgets/$pkgname.desktop "$pkgdir/usr/share/applications/$pkgname.desktop"

  install -D -m644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
 
