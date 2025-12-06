# Plasma Dialer App <img src="plasma-dialer/dialer.svg" width="40"/>

Plasma Dialer is an application that allows for making and receiving phone calls. It supports call history, contact blocking, and overlaying on top of the lockscreen (Plasma-only). It uses [ModemManager](https://modemmanager.org/) in the backend.

## Links

* Project page: https://invent.kde.org/plasma-mobile/spacebar
* File issues: https://invent.kde.org/plasma-mobile/plasma-dialer/-/issues
* Development channel: https://matrix.to/#/#plasmamobile:matrix.org

## Testing

Plasma Dialer has two daemons you need to run:

```
# ModemManager handling daemon
/usr/lib64/libexec/modem-daemon

# Call and telephony daemon
/usr/lib64/libexec/kde-telephony-daemon

# Gui app
plasma-dialer
```

### Modem Mocking

We have a testing utility in [/test/fakeserver](/test/fakeserver) that mocks the ModemManager daemon on DBus, and allows for creating fake phone calls without a modem or SIM.

```
# Stop ModemManager first
sudo systemctl stop ModemManager

# Needs to run as root
sudo plasma-dialer-fakeserver
```

This will launch a GUI application that shows the list of fake calls, and allows you trigger an incoming call.

## Developer Documentation

https://develop.kde.org/docs/plasma-mobile/
