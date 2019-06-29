# Custom Actions for Acrobat® Pro
Chun Tian (binghe)

Actions, Custom Commands, Preflights and Plugins for Adobe® Acrobat® Pro (XI and DC), writing in JavaScript and C++

## Compatibility with Adobe® Acrobat®

Tested on Acrobat® Pro XI, DC (2015) and DC (2017), both Windows and Mac versions.

Use files in "zh-cn" for Acrobat® Pro DC in Simplified Chinese; use files in "en" for Acrobat® Pro DC in English. For Acrobat® Pro in other UI languages, some keywords in the actions may have to be changed.  Most of them also work in Acrobat® Pro XI.

The (Custom) Commands are new features since DC 2015, as single-step Actions. They're not supported in XI and prior versions.

## Usage

The main entry is Action Wizards (动作向导) in Acrobat Pro, where you can import Actions and Custom Commands.

To import a toolset into Acrobat DC, simply open that file (`*.aaui`).

NOTE: Some Actions and Commands require special Preflight profile (in this repository), which must be also imported.

## Acrobat® Plugin

Certain useful tasks are impossible to write in JavaScript, because Adobe didn't expose the related APIs at JS level. In this case, the work can be done by writing plugins in C. One such plugin is in the `Plug-ins` folder.
