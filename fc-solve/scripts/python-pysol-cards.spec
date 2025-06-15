# Created by pyp2rpm-3.3.2
# This .spec was forked from the Fedora 43 python-pysol-cards.spec .
# Thanks.
%global pypi_name pysol-cards

Name:           python-%{pypi_name}
Version:        0.24.0
Release:        1%{?dist}
Summary:        Deal PySol FC Cards
License:        MIT
URL:            https://fc-solve.shlomifish.org/
Source0:        %{pypi_source pysol_cards}
BuildArch:      noarch

BuildRequires:  python%{python3_pkgversion}-devel
BuildRequires:  python%{python3_pkgversion}-setuptools

%description
The pysol-cards python module allows the python developer to generate the
initial deals of some PySol FC games.

%package -n     python%{python3_pkgversion}-%{pypi_name}
Summary:        %{summary}
%{?python_provide:%python_provide python3-%{pypi_name}}

%description -n python%{python3_pkgversion}-%{pypi_name}
The pysol-cards python module allows the python developer to generate the
initial deals of some PySol FC games.

%prep
%autosetup -n pysol_cards-%{version}
# Remove bundled egg-info
rm -rf %{pypi_name}.egg-info
sed -i '/^#! \/usr\/bin\/env python\(3\)\?$/d' pysol_cards/*.py

%build
%py3_build

%install
# Must do the default python version install last because
# the scripts in /usr/bin are overwritten with every setup.py install.
%py3_install

%files -n python%{python3_pkgversion}-%{pypi_name}
%license LICENSE
%doc README.rst
%{python3_sitelib}/pysol_cards
%{python3_sitelib}/pysol_cards-%{version}-py%{python3_version}.egg-info

%changelog
* Sat Jun 14 2025 Shlomi Fish <shlomif@shlomifish.org> 0.24.0-1
- Update to 0.24.0 (#2362883)

* Mon Jun 02 2025 Python Maint <python-maint@redhat.com> - 0.22.0-2
- Rebuilt for Python 3.14

* Fri May 02 2025 Shlomi Fish <shlomif@shlomifish.org> 0.22.0-1
- Update to 0.22.0 (#2362883)
- Remove shebangs from library .py files

* Tue Apr 29 2025 Shlomi Fish <shlomif@shlomifish.org> 0.20.0-1
- Update to 0.20.0 (#2362883)

* Wed Feb 12 2025 Shlomi Fish <shlomif@shlomifish.org> 0.18.1-1
- Update to 0.18.1 (#2344838)
- Remove no-longer-needed spec directives.

* Sat Jan 18 2025 Fedora Release Engineering <releng@fedoraproject.org> - 0.18.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_42_Mass_Rebuild

* Sun Sep 15 2024 Fedora Release Monitoring <release-monitoring@fedoraproject.org> - 0.18.0-1
- Update to 0.18.0 (#2312449)

* Fri Jul 19 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0.16.0-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_41_Mass_Rebuild

* Fri Jun 07 2024 Python Maint <python-maint@redhat.com> - 0.16.0-4
- Rebuilt for Python 3.13

* Fri Jan 26 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0.16.0-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_40_Mass_Rebuild

* Mon Jan 22 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0.16.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_40_Mass_Rebuild

* Tue Jan 09 2024 Shlomi Fish <shlomif@shlomifish.org> 0.16.0-1
- Update to the new upstream version.

* Fri Jul 21 2023 Fedora Release Engineering <releng@fedoraproject.org> - 0.14.3-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_39_Mass_Rebuild

* Wed Jun 14 2023 Python Maint <python-maint@redhat.com> - 0.14.3-3
- Rebuilt for Python 3.12

* Fri Jan 20 2023 Fedora Release Engineering <releng@fedoraproject.org> - 0.14.3-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_38_Mass_Rebuild

* Thu Jan 12 2023 Shlomi Fish <shlomif@shlomifish.org> 0.14.3-1
- Update to the new upstream version.

* Fri Jul 22 2022 Fedora Release Engineering <releng@fedoraproject.org> - 0.14.2-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_37_Mass_Rebuild

* Mon Jun 13 2022 Python Maint <python-maint@redhat.com> - 0.14.2-3
- Rebuilt for Python 3.11

* Fri Jan 21 2022 Fedora Release Engineering <releng@fedoraproject.org> - 0.14.2-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_36_Mass_Rebuild

* Sat Nov 13 2021 Shlomi Fish <shlomif@shlomifish.org> 0.14.2-1
- Update to the new upstream version.

* Fri Nov 12 2021 Shlomi Fish <shlomif@shlomifish.org> 0.14.0-1
- Update to the new upstream version.

* Thu Oct 07 2021 Shlomi Fish <shlomif@shlomifish.org> 0.12.0-1
- Update to the new upstream version.

* Fri Aug 06 2021 Fedora Release Monitoring <release-monitoring@fedoraproject.org> - 0.10.2-1
- Update to 0.10.2 (#1990823)

* Fri Jul 23 2021 Fedora Release Engineering <releng@fedoraproject.org> - 0.10.1-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_35_Mass_Rebuild

* Fri Jun 04 2021 Python Maint <python-maint@redhat.com> - 0.10.1-4
- Rebuilt for Python 3.10

* Wed Jan 27 2021 Fedora Release Engineering <releng@fedoraproject.org> - 0.10.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_34_Mass_Rebuild

* Wed Jul 29 2020 Fedora Release Engineering <releng@fedoraproject.org> - 0.10.1-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_33_Mass_Rebuild

* Sun Jun 14 2020 Shlomi Fish <shlomif@shlomifish.org> 0.10.1-1
- Update to the new upstream version.

* Sat May 30 2020 Shlomi Fish <shlomif@shlomifish.org> 0.8.18-1
- Update to the new upstream version.

* Tue May 26 2020 Miro Hrončok <mhroncok@redhat.com> - 0.8.9-4
- Rebuilt for Python 3.9

* Mon May 18 2020 Sérgio Basto <sergio@serjux.com> - 0.8.9-3
- Fix build on EPEL7

* Sat Mar 28 2020 Shlomi Fish <shlomif@cpan.org> 0.8.9-2
- Correct the date in the changelog.

* Sat Mar 28 2020 Shlomi Fish <shlomif@cpan.org> 0.8.9-1
- New upstream release.

* Thu Jan 30 2020 Fedora Release Engineering <releng@fedoraproject.org> - 0.8.7-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_32_Mass_Rebuild

* Thu Oct 03 2019 Miro Hrončok <mhroncok@redhat.com> - 0.8.6-3
- Rebuilt for Python 3.8.0rc1 (#1748018)

* Mon Aug 19 2019 Miro Hrončok <mhroncok@redhat.com> - 0.8.6-2
- Rebuilt for Python 3.8

* Fri Jul 26 2019 Fedora Release Engineering <releng@fedoraproject.org> - 0.8.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_31_Mass_Rebuild

* Mon Mar 18 2019 Shlomi Fish <shlomif@cpan.org> 0.4.1-1
- Initial Fedora package based on the Mageia one.
