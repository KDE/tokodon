# Contributing to Tokodon

Like other projects in the KDE ecosystem, contributions are welcome from all. This repository is managed in [KDE Invent](https://invent.kde.org/network/tokodon), our GitLab instance.

If this is for your first code contribution, see the [GitLab wiki page](https://community.kde.org/Infrastructure/GitLab) for a tutorial on how to send a merge request.

## Chatting

If you get stuck or need help with anything at all, head over to the [KDE New Contributors room](https://go.kde.org/matrix/#/#kde-welcome:kde.org) on Matrix. For questions about Tokodon, please ask in the [Tokodon room](https://go.kde.org/matrix/#/#tokodon:kde.org). See [Matrix](https://community.kde.org/Matrix) for more details.

## What Needs Doing

If you want to report a bug, submit it to the [KDE Bugtracking System](https://bugs.kde.org/enter_bug.cgi?format=guided&product=tokodon). Please do not use the Issues tab to report bugs.

If you found a part of Tokodon that's not translated, check out the [Getting Involved in Translation wiki page](https://community.kde.org/Get_Involved/translation) to see how you can help translate!

To finding bugs to fix or wanted features to fullfill, see the [open bugs for Tokodon](https://bugs.kde.org/buglist.cgi?list_id=2446976&product=tokodon&resolution=---) on the KDE Bugtracking System. There may be [open issues](https://invent.kde.org/network/tokodon/-/issues) on Invent as well.

## Where Stuff Is

Everything codewise for Tokodon in the [/src](/src) directory. Tokodon follows modern Kirigami and Qt practices, meaning the frontend is written in QML and the backend is C++.

### Backend

The backend code is written in C++, and we're currently requiring C++17 to build.

* Classes related to accounts are located under [/src/accounts](/src/accounts).
* The models and other classes used for the moderation tools live in [/src/admin](/src/admin).
* Code for the tests are put under [/src/autotests](/src/autotests).
* Models for the conversation (direct messages) page is located in [/src/conversation](/src/conversation).
* The backend classes for the status composer are in [/src/editor](/src/editor).
* The network controller and other miscellanous classes live in [/src/network](/src/network).
* The Purpose plugin (allowing shared to Tokodon from other KDE applications) is located under [/src/purpose](/src/purpose).
* The search models are put under [/src/search](/src/search).
* The timeline model classes live in [/src/timeline](/src/timeline).
* Everything else like generic utility classes are located in [/src/utilities](/src/utilities).

When creating new classes, please make sure they are categorized correctly. Please create a new directory if you feel like it is necessary.

### Frontend

The user interface is written in QML, using technologies such as Kirigami. Please see the [KDE API Documentation](https://api.kde.org/) for more information. The frontend files are located in [/src/content/ui](/src/content/ui).

* Generic components that can be used in many places are placed in [/src/content/ui/Components](/src/content/ui/Components).
* Parts of the moderation tool suite live in [/src/content/ui/ModerationTools](/src/content/ui/ModerationTools).
* The settings page and its related components are in [/src/content/ui/Settings](/src/content/ui/Settings).
* The composer page is located under [/src/content/ui/StatusComposer](/src/content/ui/StatusComposer).
* Timeline Posts and the many components that make up it belong in [/src/content/ui/StatusDelegate](/content/ui/StatusDelegate).
* Everything else is located in the `ui` directory for now.

The folder will eventually be reorganized, but for new components please try to put them under a relevant folder.

## Conventions

Tokodon follows the standard KDE coding style, which is enforced by the use of [clang-format](https://clang.llvm.org/docs/ClangFormat.html) and a git hook. There is nothing else you need to do, please commit any fixes the tool makes.
