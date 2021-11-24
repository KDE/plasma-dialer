# KDE Telephony Meta

D-Bus XML Interface descriptions and Qt meta types

## Project structure

* `dbus-xmls` —  D-Bus XMLs:
    + `org.kde.telephony.CallUtils`
    + `org.kde.telephony.CallHistoryDatabase`
    + `org.kde.telephony.ContactUtils.xml`
    + `org.kde.telephony.DeviceUtils.xml`
    + `org.kde.telephony.UssdUtils.xml`
    + `org.kde.telephony.VoiceMailUtils.xml`

* `meta-types` — `DialerTypes` header to share existing:
    + enums: `CallState`, `CallType`
    + struct: `CallData`

## Manually crafted D-Bus XMLs

From [Cpp2XmlPlugins - KDE TechBase](https://techbase.kde.org/Development/Tutorials/D-Bus/Cpp2XmlPlugins)
> [...] the stock qdbuscpp2xml doesn't understand non-builtin types,
> and will simply ignore any signals, slots, or properties that use them.
> In the past this meant resorting to editing the generating XML by hand,
> which defeats the whole purpose of qdbuscpp2xml.
> Below I present one strategy to overcome this deficiency, using plugins.

So, _Cpp2XmlPlugins_ approach requires rebuilding the qt-dbus and qdbuscpp2xml.
Recent real world example:
[github.com/z3ntu/razer_test/tree/master/scripts](https://github.com/z3ntu/razer_test/tree/master/scripts).

Qt Bugreports: [QTBUG-72522](https://bugreports.qt.io/browse/QTBUG-72522).

So the current approach is to fixup XML manually and share via git, not to generate automatically.

## Meta Types Headers

See also: [Using Custom Types with D-Bus - KDE Developer Documentation](https://develop.kde.org/docs/d-bus/using_custom_types_with_dbus/).

## Developer Documentation

https://develop.kde.org/docs/plasma-mobile/
