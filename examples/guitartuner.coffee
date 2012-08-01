#!/usr/bin/env coffee
# Guitar Tuner
# ------------
#
# This is a CoffeeScript and node-gir port of GNOME's guitar tuner example from
# <http://developer.gnome.org/gnome-devel-demos/unstable/guitar-tuner.js.html.en>
gir = require '../gir'
gtk = gir.load 'Gtk', '3.0'
gst = gir.load 'Gst', '0.10'

gtk.init 0
gst.init 0

guitarwindow = new gtk.Window
  type: gtk.WindowType.toplevel
  title: "Node.js Guitar Tuner"
  #  border_width: 100

guitarwindow.on 'destroy', ->
  gtk.mainQuit()
  process.exit()

guitar_box = new gtk.ButtonBox

playSound = (frequency) ->
  console.log frequency
  pipeline = new gst.Pipeline
    name: 'note'
  source = new gst.ElementFactory.make "audiotestsrc", "source"
  sink = new gst.ElementFactory.make "autoaudiosink", "output"
  source.set_property "freq", frequency
  pipeline.add source
  pipeline.add sink
  source.link sink
  pipeline.set_state gst.State.PLAYING

  #setTimeout ->
  #  pipeline.set_state gst.State.PAUSED
  #, 500

addButton = (tune, freq) ->
  button = new gtk.Button
    label: tune
  guitar_box.add button

  button.on 'clicked', ->
    playSound freq

tunes =
  E: 369.23
  A: 440
  D: 587.33
  G: 783.99
  B: 987.77
  e: 1318.5

addButton tune, freq for tune, freq of tunes
guitarwindow.add guitar_box
guitar_box.show_all()

guitarwindow.show()

gtk.main()
