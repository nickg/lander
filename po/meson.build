i18n = import('i18n')
add_project_arguments('-DGETTEXT_PACKAGE="lander"', language : 'cpp')
i18n.gettext(meson.project_name(),
             args : '--directory=' + meson.source_root())
