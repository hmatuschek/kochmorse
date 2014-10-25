Summary: A simple morse tutor using the Koch method

%define version 3.0.0

License: GPL-2.0+
Group: Applications/Communications
Name: kochmorse
Prefix: /usr
Release: 1
Source: kochmorse-%{version}.tar.gz
URL: https://github.com/hmatuschek/kochmorse
Version: %{version}
Buildroot: /tmp/kochmorserpm
BuildRequires: gcc-c++, cmake, portaudio-devel
Requires: portaudio 
%if 0%{?suse_version}
BuildRequires: libqt5-qtbase-devel, update-desktop-files
Requires: libqt5-qtbase 
%endif
%if 0%{?fedora}
BuildRequires: qt5-qtbase-devel
Requires: qt5-qtbase 
%endif

%description
A simple morse tutor using the Koch method.


%prep
%setup -q


%build
cmake -DCMAKE_BUILD_TYPE=RELEASE \
      -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
make


%install
make install
%if 0%{?suse_version}
%suse_update_desktop_file kochmorse Education Teaching
%endif

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-, root, root, -)
%attr(755, root, root) %{_prefix}/bin/kochmorse
%{_prefix}/share/applications/kochmorse.desktop
%{_prefix}/share/icons/kochmorse.svg
