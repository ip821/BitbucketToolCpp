import Foundation
import Cocoa

let statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
var statusArea: StatusArea? = nil

class StatusArea {

    init() {
        let image = NSImage(named: "status32@2x")
        image?.isTemplate = true
        statusItem.button!.image = image
        statusItem.button!.imagePosition = .imageLeft

        let statusMenu = NSMenu();
        let prefsMenuItem = NSMenuItem(title: "Maxim", action: #selector(clicked), keyEquivalent: "")
        prefsMenuItem.isEnabled = true
        prefsMenuItem.target = self
        statusMenu.addItem(prefsMenuItem)
        statusItem.menu = statusMenu
    }

    @objc func clicked() {
        let alert = NSAlert()
        alert.messageText = "Title"
        alert.informativeText = "This is a message box"
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
}

@_cdecl("swift_create_status_item") public func swift_create_status_item() {
    statusArea = StatusArea();
}
