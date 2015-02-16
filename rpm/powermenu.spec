Name:       powermenu

# >> macros
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    PowerMenu
Version:    0.4.5
Release:    1
Group:      Qt/Qt
License:    WTFPL
URL:        https://openrepos.net/content/coderus/powermenu
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   sailfish-version >= 1.1.1
Requires:   patch
Requires(pre):  patch
Requires(pre):  /usr/bin/patch
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

Summary: fancy menu and configuration for power key actions

%description
Powermenu - fancy menu and configuration for power key actions


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

%qtc_qmake5 

%qtc_make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
# << install post

%pre
# >> pre

if /sbin/pidof powermenu-daemon > /dev/null; then
killall powermenu-daemon || true
fi

if /sbin/pidof powermenu-gui > /dev/null; then
killall powermenu-gui || true
fi
# << pre

%post
# >> post
patch -p 1 -i /usr/share/powermenu-gui/data/lipstick.patch -d / || true
# << post

%preun
# >> preun
if [ $1 = 0 ]; then
patch -R -p 1 -i /usr/share/powermenu-gui/data/lipstick.patch -d / || true
fi

if /sbin/pidof powermenu-daemon > /dev/null; then
killall powermenu-daemon || true
fi

if /sbin/pidof powermenu-gui > /dev/null; then
killall powermenu-gui || true
fi
# << preun

%files
%defattr(-,root,root,-)
%{_bindir}/powermenu-daemon
%{_bindir}/powermenu-gui
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
%{_datadir}/powermenu-gui
/usr/share/dbus-1/services/org.coderus.powermenu.service
/usr/share/lipstick-jolla-home-qt5/PowerMenuDialog.qml
/usr/lib/systemd/user/powermenu.service
/usr/lib/qt5/qml/org/coderus/desktopfilemodel
# >> files
# << files
