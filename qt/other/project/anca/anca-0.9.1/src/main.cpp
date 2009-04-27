#include <ptlib.h>
#include <qapplication.h>
#include <qmessagebox.h>

#include "ancaform.h"
#include "anca.h"
#include "slavethread.h"
#include "ancaconf.h"
#include "ancainfo.h"
#include "infomanager.h"
#include "pluginfactory.h"
#include "guiproxy.h"
#include "guimanager.h"
#include "config.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

int main( int argc, char **argv, char **envp ) {
	int ret = 0;

	PProcess::PreInitialise(argc, argv, envp);
	
	// Create PProcess
	//Anca *anca = new Anca;
	Anca anca_static;
	Anca *anca = &anca_static;

	PConfigArgs args(anca->GetArguments());

	args.Parse(
		"B-forward-busy:"       "-no-forward-busy."
		"f-fast-disable."       "-no-fast-disable."
		"F-forward-always:"     "-no-forward-always."
#ifndef ENTERPRISE
		"g-gatekeeper:"
		"G-gatekeeper-id:"
		"n-no-gatekeeper."
#endif
		"h-help."
		"N-forward-no-answer:"  "-no-forward-no-answer."
		"-answer-timeout:"      "-no-answer-timeout."
#if PTRACING
		"o-output:"             "-no-output."
#endif
#ifndef ENTERPRISE
		"-password:"            "-no-password."
#endif
		"-listenport:"          "-no-listenport."
		"S-disable-h245-in-setup." "-no-disable-h245-in-setup."
		"s-sound:"              "-no-sound."
		"-sound-in:"            "-no-sound-in."
		"-sound-out:"           "-no-sound-out."
#ifdef PMEMORY_CHECK
		"-setallocationbreakpoint:"
#endif
		"T-h245tunneldisable."  "-no-h245tunneldisable."
#if PTRACING
		"t-trace."              "-no-trace."
#endif
		"u-user:"               "-no-user."
		"-videotransmit."       "-no-videotransmit."
		"-videolocal."          "-no-videolocal."
		"-videosize:"           "-no-videosize."
		"-videoformat:"         "-no-videoformat."
		"-videoinput:"          "-no-videoinput."
		"-videodevice:"         "-no-videodevice."
		"-videoimage:"
		
		"-videoreceive."        "-no-videoreceive."

		, FALSE);

#if PMEMORY_CHECK
	if (args.HasOption("setallocationbreakpoint"))
		PMemoryHeap::SetAllocationBreakpoint(args.GetOptionString("setallocationbreakpoint").AsInteger());
#endif

	cout << anca->GetName()
		<< " Version " << anca->GetVersion(TRUE)
		<< " by " << anca->GetManufacturer()
		<< " on " << anca->GetOSClass() << ' ' << anca->GetOSName()
		<< " (" << anca->GetOSVersion() << '-' << anca->GetOSHardware() << ")\n\n";

#if PTRACING
	PTrace::Initialise(args.GetOptionCount('t'),
		args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
		PTrace::Blocks | /*PTrace::Timestamp | */PTrace::Thread | PTrace::FileAndLine);
#endif

	if (args.HasOption('h') ) {
		cout << "Usage : " << anca->GetName() << " [options] [hostname/alias]\n"
			"\n   where:  hostname/alias = Remote host/alias to call\n"
			"\nOptions:\n"
			"  -h --help               : Display this help message.\n"
			
#ifndef ENTERPRISE
			"\nGatekeeper options:\n"
			"  -g --gatekeeper host    : Specify gatekeeper host.\n"
			"  -G --gatekeeper-id name : Specify gatekeeper by ID.\n"
			"  -n --no-gatekeeper      : Disable gatekeeper discovery.\n"
			"     --password pwd       : Password for gatekeeper H.235 authentication.\n"
#endif
			
			"\nDivert options:\n"
			"  -F --forward-always party    : Forward to remote party.\n"
			"  -B --forward-busy party      : Forward to remote party if busy.\n"
			"  -N --forward-no-answer party : Forward to remote party if no answer.\n"
			"     --answer-timeout time     : Time in seconds till forward on no answer.\n"

			"\nProtocol options:\n"
			"  --listenport            : Port to listen on for incoming connections (default 1720)\n"
			"  --no-listenport         : No listen port\n"
			"  -f --fast-disable       : Disable fast start\n"
			"  -T --h245tunneldisable  : Disable H245 tunnelling.\n"
			"  -u --user name          : Set local alias name(s) (defaults to login name)\n"
			"  -S --disable-h245-in-setup Disable H245 in setup\n"

			"\nVideo transmit options:\n"
			"  --videodevice dev       : Select video capture device\n"
			"  --no-videotransmit      : Disable video transmission\n"
			"  --no-videolocal         : Disable local video window\n"
			"  --videosize size        : Sets size of transmitted video window\n"
			"                             size can be small (default) or large\n"
			"  --videoformat type      : Set capture video format\n"
			"                             can be auto (default) pal or ntsc\n"
			"  --videoimage image      : Image to show when video device is not set correctly\n"
			"\nVideo receive options:\n"
			"  --no-videoreceive       : Disable video reception\n"

			"\nSound card options:\n"
			"  -s --sound device       : Select sound card input/output device\n"
			"  --sound-in device       : Select sound card input device (overrides --sound)\n"
			"  --sound-out device      : Select sound card output device (overrides --sound)\n"

#if PTRACING || PMEMORY_CHECK
			"\nDebug options:\n"
#endif
#if PTRACING
		        "  -t --trace              : Enable trace, use multiple times for more detail\n"
		        "  -o --output             : File for trace output, default is stderr\n"
#endif
#ifdef PMEMORY_CHECK
		        "  --setallocationbreakpoint n : Enable breakpoint on memory allocation n\n"
#endif
		        << endl;
		return 0;
	}

#ifdef CONFIGDIR
	AncaConf *ancaConf = new AncaConf( CONFIGDIR, ".anca" );
#else
	AncaConf ancaConf( "/etc/anca", ".anca" );
#endif
	
	// Create info class
	PTRACE(3, "Creating AncaInfo");
	AncaInfo *ancaInfo = new AncaInfo;
	
	// Create manager of AncaInfo
	PTRACE(3, "Creating InfoManager");
	InfoManager *infoManager = new InfoManager;;
	
	// Create plugin factory
	PTRACE(3, "Creating PluginFactory");
	PluginFactory *pluginFactory = new PluginFactory(args);

	// Create second - slave thread
	PTRACE(3, "Creating SlaveThread");
	SlaveThread *slaveThread = new SlaveThread(args);
	slaveThread->Resume();

	// Create main form and show it
#ifdef Q_WS_QWS
	PTRACE(3, "Creating QApplication");
	QApplication *a = QApplication( argc, argv, QApplication::GuiServer );
	PTRACE(3, "Creating AncaForm");
	AncaForm *w = new AncaForm( 0, "ancaForm", Qt::WStyle_Customize | Qt::WStyle_NoBorder);
#ifndef ENTERPRISE
	w->showFullScreen();
#endif //not ENTERPRISE
#else
	PTRACE(3, "Creating QApplication");
	QApplication *a = new QApplication( argc, argv);
	PTRACE(3, "Creating AncaForm");
	AncaForm *w = new AncaForm( 0, "ancaForm");
#ifndef ENTERPRISE
	w->show();
#endif //not ENTERPRISE
#endif //not Q_WS_QWS
	a->setMainWidget(w);
	a->connect( a, SIGNAL( lastWindowClosed() ), a, SLOT( quit() ) );

	// initialize InfoManager - connect some basic notifiers
	PTRACE(3, "Initializing InfoManager");
	infoManager->init();

	// load all required plugins
	PTRACE(3, "Loading plugins");
	pluginFactory->load();

	// Create GUI manager
	PTRACE(3, "Creating GUIManager");
	GUIManager *guiManager = new GUIManager;

	// Create GUI proxy
	PTRACE(3, "Creating GUIProxy");
	GUIProxy *guiProxy = new GUIProxy;
	
	// Initialize openh323 part
	PTRACE(3, "Initializing Anca");
	if (!anca->init(args)) {
		QMessageBox::critical( 0, "Anca",
			"Unable to initialize Anca subsystem.\n"
			"Application will now exit." );
		ret = EXIT_FAILURE;
		goto out;
	}

	// Create GUIs plugins provide.
	// It's separated from loading of plugins because we need openh323 part initialized
	PTRACE(3, "Creating GUIs of plugins");
	pluginFactory->createGUIs();
	guiManager->setColors();
	
	//show main window if ENTERPRISE
#ifdef ENTERPRISE
#ifdef Q_WS_QWS
	w->showFullScreen();
#else
	w->show();
#endif //Q_WS_QWS
#endif //ENTERPRISE

	// Run Qt
	ret = a->exec();
	PTRACE(3, "AncaForm exited");

out:
	// Shutdown
	anca->shutdown();

	PTRACE(3, "Deleting GUIManager");
	delete guiManager;
	PTRACE(3, "Deleting GUIProxy");
	delete guiProxy;
	PTRACE(3, "Deleting PluginFactory");
	delete pluginFactory;

	slaveThread->doInstruction(SlaveThread::Terminate);
	slaveThread->WaitForTermination();
	PTRACE(3, "Deleting SlaveThread");
	delete slaveThread;

	PTRACE(3, "Deleting AncaForm");
	delete w;

	PTRACE(3, "Deleting InfoManager");
	delete infoManager;
	PTRACE(3, "Deleting AncaInfo");
	delete ancaInfo;
	PTRACE(3, "Deleting AncaConf");
	delete ancaConf;

	PTRACE(3, "Deleting QApplication");
	delete a;

//	PTRACE(3, "Deleting Anca");
//	delete anca;

	PTRACE(3, "End of program");
	return ret;
}
