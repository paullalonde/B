// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

// file header
#include "BWindow.h"

// B headers
#include "BAEObjectSupport.h"
#include "BAEReader.h"
#include "BAEWriter.h"
#include "BBundle.h"
#include "BEvent.h"
#include "BException.h"
#include "BIcon.h"
#include "BUtility.h"
#include "BView.h"
#include "BWindowProperty.h"
#include "BWindowUtils.h"


namespace {

	typedef B::WindowProperty<B::kEventClassB, 'Wind', B::Window*>	WindowObjectProperty;
	
	/*!	@brief	Functor that recursively prints debug info for all views.
	*/
	class DebugPrinter : public std::unary_function<void, HIViewRef>
	{
	public:
				DebugPrinter(int inLevel)
					: mLevel(inLevel) {}
		
		void	operator () (HIViewRef viewRef) const;
		
	private:
		int	mLevel;
	};
	
	void
	DebugPrinter::operator () (HIViewRef viewRef) const
	{
		printf("### LEVEL %d ###\n", mLevel);
		fflush(stdout);
		
		HIObjectPrintDebugInfo((HIObjectRef) viewRef);
		fflush(stdout);
		
		std::for_each(B::ViewUtils::begin(viewRef), B::ViewUtils::end(viewRef), 
					  DebugPrinter(mLevel+1));
	}
	
	class Awakener : public std::unary_function<void, HIViewRef>
	{
	public:
				Awakener(B::Nib* nib)
					: mNib(nib) {}
				Awakener(const Awakener& orig)
					: mNib(orig.mNib) {}
		
		void	operator () (HIViewRef viewRef) const
                    {
                        B::View::AwakenViewAndChildren(viewRef, mNib);
                    }
				
	private:
		B::Nib*	mNib;
	};

}	// anonymous namespace


namespace B {

// ==========================================================================================
//	Window

std::list< boost::shared_ptr<Window> >	Window::sWindows;

// ------------------------------------------------------------------------------------------
/*! Obviously, this constructor can only be called for @c WindowRefs that already exist!
*/
Window::Window(
	WindowRef		inWindowRef,				//!< An already-existing @c WindowRef.
	AEObjectPtr		inContainer,				//!< The window's AEOM container.
	DescType		inClassID /* = cWindow */)  //!< The window's AEOM class.
		: AEObject(inContainer, inClassID), 
		  mWindowRef(inWindowRef), 
		  mEventHandler(inWindowRef), 
		  mContentView(NULL), mIdealSize(0.0f, 0.0f),
		  mComposited(WindowUtils::IsComposited(inWindowRef)), 
		  mHandlingAECollapse(false), mHandlingAEZoom(false)
{
printf("ctor B::Window %p\n", this);

	B_ASSERT(mWindowRef != NULL);
	// If this asserts, we are probably trying to instantiate more than one Window 
	// for a given @c WindowRef.
	B_ASSERT(!WindowObjectProperty::Exists(mWindowRef));
	
	OSStatus	err;
	
	// Install our custom window property.
	
	Window*	window	= this;
	
	WindowObjectProperty::Set(inWindowRef, window);
	
	// Retrieve the window's content view.
	// 
	// According to Apple Technical Q&A 1214:
	// 
	//	"Windows only have a Root Control if one is explicitly created by 
	//	CreateRootControl or implicitly created by calling a CreateFooControl API 
	//	with a validWindowRef."
	//
	// So if we aren't composited, make sure we have a root control.
	
	if (!mComposited)
	{
		err = GetRootControl(mWindowRef, &mContentView);
		
		if ((err != noErr) || (mContentView == NULL))
		{
			err = CreateRootControl(mWindowRef, &mContentView);
			B_THROW_IF_STATUS(err);
		}
	}
	else
	{
		// Composited window:  get the real content view.
		
		err = HIViewFindByID(HIViewGetRoot(mWindowRef), kHIViewWindowContentID, 
							 &mContentView);
		B_THROW_IF_STATUS(err);
	}
	
//	HIObjectPrintDebugInfo(reinterpret_cast<HIObjectRef>(mContentView));
	
	// Install event handlers.
	
	InitEventHandler();
	
	// Set up drag & drop.
	
	err = SetAutomaticControlDragTrackingEnabledForWindow(mWindowRef, true);
	B_THROW_IF_STATUS(err);
	
	// Bump our reference count.
	RetainWindow(mWindowRef);
}

// ------------------------------------------------------------------------------------------
Window::~Window()
{
	B_ASSERT(mWindowRef == NULL);

printf("dtor B::Window %p\n", this);
	
	RemoveFromWindowList(std::nothrow);
}

// ------------------------------------------------------------------------------------------
void
Window::operator delete (void* ptr)
{
	::operator delete (ptr);
}

// ------------------------------------------------------------------------------------------
void
Window::InitEventHandler()
{
	mEventHandler.Add(this, &Window::WindowShowing);
	mEventHandler.Add(this, &Window::WindowHiding);
	mEventHandler.Add(this, &Window::WindowShown);
	mEventHandler.Add(this, &Window::WindowHidden);
	mEventHandler.Add(this, &Window::WindowActivated);
	mEventHandler.Add(this, &Window::WindowDeactivated);
	mEventHandler.Add(this, &Window::WindowZoomed);
	mEventHandler.Add(this, &Window::WindowResizeCompleted);
	mEventHandler.Add(this, &Window::WindowDragCompleted);
	mEventHandler.Add(this, &Window::WindowTransitionCompleted);
	mEventHandler.Add(this, &Window::WindowCollapsed);
	mEventHandler.Add(this, &Window::WindowExpanded);
	mEventHandler.Add(this, &Window::WindowClose);
	mEventHandler.Add(this, &Window::WindowClosed);
	mEventHandler.Add(this, &Window::WindowGetIdealSize);
	mEventHandler.Add(this, &Window::WindowFocusAcquired);
	mEventHandler.Add(this, &Window::WindowFocusRelinquish);
	mEventHandler.Add(this, &Window::WindowSheetOpened);
	mEventHandler.Add(this, &Window::WindowSheetClosed);
	mEventHandler.Add(this, &Window::CommandProcess);
	mEventHandler.Add(this, &Window::CommandUpdateStatus);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
Window*
Window::FromWindowRef(
	WindowRef		inWindowRef)
{
	Window*	window;
	
	if (!WindowObjectProperty::Get(inWindowRef, window, std::nothrow))
		window = NULL;
	
	return (window);
}

// ------------------------------------------------------------------------------------------
void
Window::CloseWindow(
	OSType				/* inSaveOption */ /* = kAEAsk */, 
	const Url&			/* inUrl */ /* = Url() */)
{
	if (ShouldWindowClose())
	{
		Close();
	}
}

// ------------------------------------------------------------------------------------------
void
Window::PostCreateWindow(Nib* inFromNib)
{
	RegisterViews();
	InstantiateViews();
	AwakenViews(inFromNib);
	Awaken(inFromNib);
}

// ------------------------------------------------------------------------------------------
void
Window::RegisterViews()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
View*
Window::Instantiate(HIViewRef inViewRef) const
{
	return (mViewFactory.Instantiate(inViewRef));
}

// ------------------------------------------------------------------------------------------
void
Window::InstantiateViews()
{
	if (IsComposited())
	{
		mViewFactory.Instantiate(mContentView);
	}
	
	View::InstantiateSubviews(mContentView, mViewFactory);
				  
}

// ------------------------------------------------------------------------------------------
void
Window::AwakenViews(Nib* inFromNib)
{
	std::for_each(ViewUtils::begin(mContentView), ViewUtils::end(mContentView), 
				  Awakener(inFromNib));
}

// ------------------------------------------------------------------------------------------
/*!	At the time this function is called, all subviews of the window have already been 
	awaken, so it's safe to access them.
	
	Derived classes may override this if they need to perform some processing once their 
	views are accessible, or if they need access to the nib from which the window 
	was loaded.
	
	The default implementation does nothing.  There's no point in calling it from other 
	classes, so it's been made private.
	
	@note	@a inNib is non-@c NULL if the window was loaded from a nib.  If the window was 
			created programatically, it will be @c NULL.  Also, @a inNib is only valid for 
			the duration of this function invocation.  Derived classes should @b not 
			save a reference to it.
*/
void
Window::Awaken(
	Nib*	/* inFromNib */)	//!< The nib from which to load other resources.  May be @c NULL.
{
}

// ------------------------------------------------------------------------------------------
bool
Window::ShouldWindowClose()
{
	return (true);
}

// ------------------------------------------------------------------------------------------
void
Window::Select()
{
	SelectWindow(mWindowRef);
}

// ------------------------------------------------------------------------------------------
void
Window::Show(bool showIt)
{
	if (showIt)
		ShowWindow(mWindowRef);
	else
		HideWindow(mWindowRef);
}

// ------------------------------------------------------------------------------------------
void
Window::Transition(
	WindowTransitionEffect	inEffect, 
	WindowTransitionAction	inAction, 
	Rect*					inRect /* = NULL */, 
	Window*					inParent /* = NULL */)
{
	::Rect*		qdRectPtr	= NULL;
	::Rect		qdRect;
	OSStatus	err;
	
	if (inRect != NULL)
	{
		qdRect = Rect::QDRect(*inRect);
		qdRectPtr = &qdRect;
	}
	
	if (inParent != NULL)
	{
		err = TransitionWindowAndParent(mWindowRef, *inParent, 
										inEffect, inAction, qdRectPtr);
	}
	else
	{
		err = TransitionWindow(mWindowRef, inEffect, inAction, qdRectPtr);
	}
	
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::Reposition(
	WindowPositionMethod	inMethod, 
	Window*					inParent /* = NULL */)
{
	OSStatus	err;
	
	err = RepositionWindow(mWindowRef, *inParent, inMethod);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::Close()
{
	if (IsInWindowList())
		RemoveFromWindowList();
	else
		ReleaseWindow(mWindowRef);
}

// ------------------------------------------------------------------------------------------
String
Window::GetName() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = CopyWindowTitleAsCFString(mWindowRef, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
void
Window::SetTitle(
	const String&	inTitle)
{
	OSStatus	err;
	
	err = SetWindowTitleWithCFString(mWindowRef, inTitle.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::SetAlternateTitle(
	const String&	inTitle)
{
	OSStatus	err;
	
	err = SetWindowAlternateTitle(mWindowRef, inTitle.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::SetProxy(
	const Icon&	inIcon)
{
	OSStatus	err;
	
	err = SetWindowProxyIcon(mWindowRef, inIcon);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::SetProxy(
	const Url&	inUrl)
{
    FSRef		ref;
    OSStatus	err;
    
    inUrl.Copy(ref);
    
    err = HIWindowSetProxyFSRef(mWindowRef, &ref);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::SetProxy(
	OSType		inCreator, 
	OSType		inFileType)
{
	OSStatus	err;
	
	err = SetWindowProxyCreatorAndType(mWindowRef, inCreator, 
									   inFileType, kOnSystemDisk);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::ClearProxy()
{
	OSStatus	err;
	
	err = RemoveWindowProxy(mWindowRef);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::SetModified(
	bool		inModified)
{
	OSStatus	err;
	
	err = SetWindowModified(mWindowRef, inModified);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Call this when your window's minimum size is fixed.  If it's not (i.e. it changes 
	dynamically), you should install a handler on 
	kEventClassWindow/kEventWindowGetMinimumSize instead.
*/
void
Window::SetMinimumSize(
	const Size&	inMinimumSize)
{
	OSStatus	err;
	
	err = SetWindowResizeLimits(mWindowRef, &inMinimumSize, NULL);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Call this when your window's maximum size is fixed.  If it's not (i.e. it 
	changes dynamically), you should install a handler on 
	kEventClassWindow/kEventWindowGetMaximumSize instead.
*/
void
Window::SetMaximumSize(
	const Size&	inMaximumSize)
{
	OSStatus	err;
	
	err = SetWindowResizeLimits(mWindowRef, NULL, &inMaximumSize);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Call this when your window's ideal size is fixed.  If it's not (i.e. it 
	changes dynamically), you should override GetIdealSize() instead.
*/
void
Window::SetIdealSize(
	const Size&	inIdealSize)
{
	mIdealSize = inIdealSize;
}

// ------------------------------------------------------------------------------------------
void
Window::SetToolbar(OSPtr<HIToolbarRef> inToolbar)
{
	OSStatus	err;
	
	err = SetWindowToolbar(mWindowRef, inToolbar);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::ShowToolbar(bool inShowIt, bool inAnimate)
{
	OSStatus	err;
	
	err = ShowHideWindowToolbar(mWindowRef, inShowIt, inAnimate);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsToolbarVisible() const
{
	return (IsWindowToolbarVisible(mWindowRef));
}

// ------------------------------------------------------------------------------------------
bool
Window::IsZoomable() const
{
	WindowAttributes	attributes;
	OSStatus			err;
	
	err = GetWindowAttributes(mWindowRef, &attributes);
	B_THROW_IF_STATUS(err);
	
	return ((attributes & kWindowFullZoomAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsZoomed() const
{
	bool	isZoomed	= false;
	
	if (IsZoomable())
	{
		Size	idealSize	= GetIdealSize();
		::Point	idealSizePt	= { static_cast<short>(idealSize.height), static_cast<short>(idealSize.width) };
		::Rect	idealState;
		
		isZoomed = IsWindowInStandardState(mWindowRef, &idealSizePt, &idealState);
	}
	
	return (isZoomed);
}

// ------------------------------------------------------------------------------------------
void
Window::Zoom(
	bool			inZoomItOut)
{
	WindowPartCode	zoomPart	= inZoomItOut ? inZoomOut : inZoomIn;
	Size			idealSize	= GetIdealSize();
	::Point			idealSizePt	= { static_cast<short>(idealSize.height), static_cast<short>(idealSize.width) };
	OSStatus		err;
	
	err = ZoomWindowIdeal(mWindowRef, zoomPart, &idealSizePt);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsCollapseable() const
{
	WindowAttributes	attributes;
	OSStatus			err;
	
	err = GetWindowAttributes(mWindowRef, &attributes);
	B_THROW_IF_STATUS(err);
	
	return ((attributes & kWindowCollapseBoxAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsCollapsed() const
{
	return (IsWindowCollapsed(mWindowRef));
}

// ------------------------------------------------------------------------------------------
void
Window::Collapse(
	bool			inCollapse)
{
	OSStatus	err;
	
	err = CollapseWindow(mWindowRef, inCollapse);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsCloseable() const
{
	WindowAttributes	attributes;
	OSStatus			err;
	
	err = GetWindowAttributes(mWindowRef, &attributes);
	B_THROW_IF_STATUS(err);
	
	return ((attributes & kWindowCloseBoxAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsResizeable() const
{
	WindowAttributes	attributes;
	OSStatus			err;
	
	err = GetWindowAttributes(mWindowRef, &attributes);
	B_THROW_IF_STATUS(err);
	
	return ((attributes & kWindowResizableAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsFloating() const
{
	WindowClass	windowClass;
	OSStatus	err;
	
	err = GetWindowClass(mWindowRef, &windowClass);
	B_THROW_IF_STATUS(err);
	
	return (windowClass == kFloatingWindowClass);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsModal() const
{
	WindowClass	windowClass;
	OSStatus	err;
	
	err = GetWindowClass(mWindowRef, &windowClass);
	B_THROW_IF_STATUS(err);
	
	return ((windowClass == kModalWindowClass) || 
			(windowClass == kMovableModalWindowClass));
}

// ------------------------------------------------------------------------------------------
Size
Window::GetIdealSize() const
{
	Size	size;
	
	if (mIdealSize.width == 0.0f)
	{
		::Rect	bounds;
		
		GetRegionBounds(GetGrayRgn(), &bounds);
		
		size.width	= bounds.right - bounds.left;
		size.height	= bounds.bottom - bounds.top;
	}
	else
	{
		size = mIdealSize;
	}
	
	return (size);
}

// ------------------------------------------------------------------------------------------
Rect
Window::GetContentViewBounds() const
{
	return (WindowUtils::GetContentViewBounds(mWindowRef, IsComposited()));
}

// ------------------------------------------------------------------------------------------
Rect
Window::GetStructureBounds() const
{
	return (WindowUtils::GetStructureBounds(mWindowRef));
}

// ------------------------------------------------------------------------------------------
Point
Window::GetStructureOrigin() const
{
	return (WindowUtils::GetStructureOrigin(mWindowRef));
}

// ------------------------------------------------------------------------------------------
Size
Window::GetStructureSize() const
{
	return (WindowUtils::GetStructureSize(mWindowRef));
}

// ------------------------------------------------------------------------------------------
Rect
Window::GetContentBounds() const
{
	return (WindowUtils::GetContentBounds(mWindowRef));
}

// ------------------------------------------------------------------------------------------
Point
Window::GetContentOrigin() const
{
	return (WindowUtils::GetContentOrigin(mWindowRef));
}

// ------------------------------------------------------------------------------------------
Size
Window::GetContentSize() const
{
	return (WindowUtils::GetContentSize(mWindowRef));
}

// ------------------------------------------------------------------------------------------
void
Window::SetStructureBounds(
	const Rect&		inBounds)
{
	::Rect		qdBounds	= inBounds;
	OSStatus	err;
	
	err = SetWindowBounds(mWindowRef, kWindowStructureRgn, &qdBounds);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::ResizeStructureTo(
	const Size&		inSize)
{
	Rect	bounds	= GetStructureBounds();
	
	bounds.size = inSize;
	
	SetStructureBounds(bounds);
}

// ------------------------------------------------------------------------------------------
void
Window::ResizeStructureBy(
	float			inDeltaWidth, 
	float			inDeltaHeight)
{
	Rect	bounds	= GetStructureBounds();
	
	bounds.size.width	+= inDeltaWidth;
	bounds.size.height	+= inDeltaHeight;
	
	SetStructureBounds(bounds);
}

// ------------------------------------------------------------------------------------------
void
Window::MoveStructureTo(
	const Point&	inOrigin)
{
	OSStatus	err;
	
	err = MoveWindowStructure(mWindowRef, static_cast<short>(inOrigin.x), static_cast<short>(inOrigin.y));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::MoveStructureBy(
	float			inDeltaX, 
	float			inDeltaY)
{
	Rect	bounds	= GetStructureBounds();
	
	bounds.origin.x	+= inDeltaX;
	bounds.origin.y	+= inDeltaY;
	
	SetStructureBounds(bounds);
}

// ------------------------------------------------------------------------------------------
void
Window::SetContentBounds(
	const Rect&		inBounds)
{
	::Rect		qdBounds	= inBounds;
	OSStatus	err;
	
	err = SetWindowBounds(mWindowRef, kWindowContentRgn, &qdBounds);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::ResizeContentTo(
	const Size&		inSize)
{
	SizeWindow(mWindowRef, static_cast<short>(inSize.width), static_cast<short>(inSize.height), true);
}

// ------------------------------------------------------------------------------------------
void
Window::ResizeContentBy(
	float			inDeltaWidth, 
	float			inDeltaHeight)
{
	Rect	bounds	= GetStructureBounds();
	
	bounds.size.width	+= inDeltaWidth;
	bounds.size.height	+= inDeltaHeight;
	
	SetContentBounds(bounds);
}

// ------------------------------------------------------------------------------------------
void
Window::MoveContentTo(
	const Point&	inOrigin)
{
	MoveWindow(mWindowRef, static_cast<short>(inOrigin.x), static_cast<short>(inOrigin.y), false);
}

// ------------------------------------------------------------------------------------------
void
Window::MoveContentBy(
	float			inDeltaX, 
	float			inDeltaY)
{
	Rect	bounds	= GetStructureBounds();
	
	bounds.origin.x	+= inDeltaX;
	bounds.origin.y	+= inDeltaY;
	
	SetContentBounds(bounds);
}

// ------------------------------------------------------------------------------------------
bool
Window::IsInWindowList() const
{
	boost::shared_ptr<Window const>	windowPtr	= boost::static_pointer_cast<Window const>(GetAEObjectPtr());
	
	return (std::find(sWindows.begin(), sWindows.end(), windowPtr) != sWindows.end());
}

// ------------------------------------------------------------------------------------------
void
Window::AddToWindowList()
{
//printf("window list: adding %p [%u]\n", this, sWindows.size());

	sWindows.push_back(boost::static_pointer_cast<Window>(GetAEObjectPtr()));
}

// ------------------------------------------------------------------------------------------
void
Window::RemoveFromWindowList()
{
//printf("window list: removing %p [%u]\n", this, sWindows.size());

	sWindows.remove(boost::static_pointer_cast<Window>(GetAEObjectPtr()));
}

// ------------------------------------------------------------------------------------------
void
Window::RemoveFromWindowList(const std::nothrow_t&)
{
	// If the window is released via the Window Manager (via ReleaseWindow or DisposeWindow), 
	// then our Close() function won't have been called, so we need to ensure here that 
	// our shared pointer doesn't hang around in the global list.
	
	if (!_internal_weak_this.expired())
	{
		RemoveFromWindowList();
	}
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowShowing(
	Event<kEventClassWindow, kEventWindowShowing>&	/* event */)
{
	HandleShowing();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowHiding(
	Event<kEventClassWindow, kEventWindowHiding>&	/* event */)
{
	HandleHiding();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowShown(
	Event<kEventClassWindow, kEventWindowShown>&	/* event */)
{
	HandleShown();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowHidden(
	Event<kEventClassWindow, kEventWindowHidden>&	/* event */)
{
	HandleHidden();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowActivated(
	Event<kEventClassWindow, kEventWindowActivated>&	/* event */)
{
	HandleActivated();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowDeactivated(
	Event<kEventClassWindow, kEventWindowDeactivated>&	/* event */)
{
	HandleDeactivated();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowZoomed(
	Event<kEventClassWindow, kEventWindowZoomed>&	/* event */)
{
	return (HandleZoomed());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowResizeCompleted(
	Event<kEventClassWindow, kEventWindowResizeCompleted>&	/* event */)
{
	return (HandleResizeCompleted());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowDragCompleted(
	Event<kEventClassWindow, kEventWindowDragCompleted>&	/* event */)
{
	return (HandleDragCompleted());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowTransitionCompleted(
	Event<kEventClassWindow, kEventWindowTransitionCompleted>&	event)
{
	return (HandleTransitionCompleted(event.mUserData));
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowCollapsed(
	Event<kEventClassWindow, kEventWindowCollapsed>&	/* event */)
{
	return (HandleCollapsed());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowExpanded(
	Event<kEventClassWindow, kEventWindowExpanded>&	/* event */)
{
	return (HandleExpanded());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowClose(
	Event<kEventClassWindow, kEventWindowClose>&	/* event */)
{
	CloseWindow();
	
	return (true);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowClosed(
	Event<kEventClassWindow, kEventWindowClosed>&	/* event */)
{
	WindowObjectProperty::Remove(mWindowRef);
	
	mWindowRef = NULL;
	
	delete this;
	
	return (true);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowGetIdealSize(
	Event<kEventClassWindow, kEventWindowGetIdealSize>&	event)
{
	Size	ideal	= GetIdealSize();
	
	Event<kEventClassWindow, kEventWindowGetMinimumSize>	minSizeEvent(*this);
	Event<kEventClassWindow, kEventWindowGetMaximumSize>	maxSizeEvent(*this);
	
	if (minSizeEvent.Send(*this, std::nothrow) == noErr)
	{
		ideal.width  = std::max(ideal.width,  minSizeEvent.mDimensions.width);
		ideal.height = std::max(ideal.height, minSizeEvent.mDimensions.height);
	}
	
	if (maxSizeEvent.Send(*this, std::nothrow) == noErr)
	{
		ideal.width  = std::min(ideal.width,  maxSizeEvent.mDimensions.width);
		ideal.height = std::min(ideal.height, maxSizeEvent.mDimensions.height);
	}
	
	event.mDimensions = ideal;
	
	return (true);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowFocusAcquired(
	Event<kEventClassWindow, kEventWindowFocusAcquired>&	/* event */)
{
	return (HandleFocusAcquired());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowFocusRelinquish(
	Event<kEventClassWindow, kEventWindowFocusRelinquish>&	/* event */)
{
	return (HandleFocusRelinquish());
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowSheetOpened(
	Event<kEventClassWindow, kEventWindowSheetOpened>&	event)
{
	if (event.mWindowRef != GetWindowRef())
		HandleSheetOpened(event.mWindowRef);
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::WindowSheetClosed(
	Event<kEventClassWindow, kEventWindowSheetClosed>&	event)
{
	if (event.mWindowRef != GetWindowRef())
		HandleSheetClosed(event.mWindowRef);
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	return (HandleCommand(event.mHICommand));
}

// ------------------------------------------------------------------------------------------
bool
Window::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	return (HandleUpdateStatus(event.mHICommand, event.mData));
}

// ------------------------------------------------------------------------------------------
void
Window::HandleShowing()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleHiding()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleShown()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleHidden()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleActivated()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleDeactivated()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleFocusAcquired()
{
	// Default implementation does nothing.
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleFocusRelinquish()
{
	// Default implementation does nothing.
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleZoomed()
{
	if (!mHandlingAEZoom)
	{
		// Send ourselves an Apple Event recording the new zoom state.  We don't 
		// execute the event, since this is a user-initiated zoom and the 
		// window is already at the new position.
		
		SendSetPropertyAppleEvent<typeBoolean>(pIsZoomed, IsZoomed(), kAEDontExecute);
	}

	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleResizeCompleted()
{
	// Send ourselves an Apple Event recording the new size.  We don't execute 
	// the event, since this is a user-initiated size change and the window 
	// is already at the new size.
	
	AEWriter		writer;
	AEDescriptor	propertyDesc;
	
	WriteProperty(keyAEBounds, writer);
	writer.Close(propertyDesc);
	SendSetPropertyAppleEvent(keyAEBounds, propertyDesc, kAEDontExecute);

	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleDragCompleted()
{
	// Send ourselves an Apple Event recording the new position.  We don't execute 
	// the event, since this is a user-initiated position change and the window 
	// is already at the new position.
	
	AEWriter		writer;
	AEDescriptor	propertyDesc;
	
	WriteProperty(kAESetPosition, writer);
	writer.Close(propertyDesc);
	SendSetPropertyAppleEvent(kAESetPosition, propertyDesc, kAEDontExecute);

	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleTransitionCompleted(void* /* inUserData */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleCollapsed()
{
	if (!mHandlingAECollapse)
	{
		// Send ourselves an Apple Event recording the new collapsed state.  We don't 
		// execute the event, since this is a user-initiated position change and the 
		// window is already at the new position.
		
		SendSetPropertyAppleEvent<typeBoolean>(pIsMinimized, true, kAEDontExecute);
	}
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleExpanded()
{
	if (!mHandlingAECollapse)
	{
		// Send ourselves an Apple Event recording the new collapsed state.  We don't 
		// execute the event, since this is a user-initiated position change and the 
		// window is already at the new position.
		
		SendSetPropertyAppleEvent<typeBoolean>(pIsMinimized, false, kAEDontExecute);
	}

	return (false);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandClose:
		// Handle the close by sending an Apple %Event to ourselves.
		if (IsCloseable())
			SendCloseAppleEvent();
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
Window::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandClose:
		ioCmdData.SetEnabled(IsCloseable());
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
void
Window::HandleSheetOpened(
	WindowRef	/* inSheetWindowRef */)
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::HandleSheetClosed(
	WindowRef	/* inSheetWindowRef */)
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
Window::WriteProperty(
	DescType		inPropertyID, 
	AEWriter&		ioWriter) const
{
	OSStatus	err;

	switch (inPropertyID)
	{
	case pName:
		// Window Title
		ioWriter.Write<typeUTF16ExternalRepresentation>(GetName());
		break;

#if 0
	case pIndex: {				// Front-to-back position index
		SInt32	theIndex = UWindows::FindWindowIndex(mWindowRefP);
		err = ::AECreateDesc(typeSInt32, (Ptr) &theIndex,
							sizeof(SInt32), &outPropertyDesc);
		break;
	}
#endif

	case kAESetPosition:
		ioWriter.Write<typeQDPoint>(GetStructureOrigin());
		break;

	case keyAEBounds:
		ioWriter.Write<typeQDRectangle>(GetStructureBounds());
		break;
		
	case pIsResizable:
		ioWriter.Write<typeBoolean>(IsResizeable());
		break;
		
	case pIsZoomable:
		ioWriter.Write<typeBoolean>(IsZoomable());
		break;
		
	case pIsMinimizable:
		ioWriter.Write<typeBoolean>(IsCollapseable());
		break;
		
	case pIsMinimized:
		ioWriter.Write<typeBoolean>(IsCollapsed());
		break;
		
	case pHasCloseBox:
		ioWriter.Write<typeBoolean>(IsCloseable());
		break;

	case pIsFloating:
		ioWriter.Write<typeBoolean>(IsFloating());
		break;
		
	case pIsModal:
		ioWriter.Write<typeBoolean>(IsModal());
		break;
		
	case pHasTitleBar:
		// We determine whether a window has a title bar by getting its
		// region and checking for the empty region.  Note that GetWindowRegion
		// may well return an error for a window with no title bar, so we
		// just treat that as false.
		{
			bool		hasTitle	= false;
			AutoRegion	rgn;
			
			err = GetWindowRegion(mWindowRef, kWindowTitleBarRgn, rgn);
			
			if (err == noErr)
				hasTitle = !EmptyRgn(rgn);
			
			ioWriter.Write<typeBoolean>(hasTitle);
		}
		break;
		
	case pIsZoomed:
		ioWriter.Write<typeBoolean>(IsZoomed());
		break;

	case pVisible:
		ioWriter.Write<typeBoolean>(IsVisible());
		break;

	default:
		AEObject::WriteProperty(inPropertyID, ioWriter);
		break;
	}
}

// ------------------------------------------------------------------------------------------
void
Window::ReadProperty(
	DescType		inPropertyID, 
	AEReader&		ioReader)
{
	Point	position;
	Rect	bounds;
	bool	visible;
	
	switch (inPropertyID)
	{
	case kAESetPosition:
		MoveStructureTo(ioReader.Read<typeQDPoint>(position));
		break;
		
	case keyAEBounds:
		// You can only change the size of the window if the window
		// is resizable.
		if (!IsResizeable())
			B_THROW(AENotModifiableException());
		SetStructureBounds(ioReader.Read<typeQDRectangle>(bounds));
		break;
		
	case pIsZoomed:
		if (!IsZoomable())
		{
			B_THROW(AENotModifiableException());
		}
		else
		{
			AutoValue<bool>	state(mHandlingAEZoom, true);
			bool			zoomed;
			
			Zoom(ioReader.Read<typeBoolean>(zoomed));
		}
		break;
		
	case pVisible:
		Show(ioReader.Read<typeBoolean>(visible));
		break;
		
	case pIsMinimized:
		if (!IsCollapseable())
		{
			B_THROW(AENotModifiableException());
		}
		else
		{
			AutoValue<bool>	state(mHandlingAECollapse, true);
			bool			collapsed;
			
			Collapse(ioReader.Read<typeBoolean>(collapsed));
		}
		break;
		
	default:
		AEObject::ReadProperty(inPropertyID, ioReader);
		break;
	}
}

// ------------------------------------------------------------------------------------------
void
Window::CloseObject(
	OSType			inSaveOption /* = kAEAsk */,
	const Url&		inUrl /* = Url() */)
{
	CloseWindow(inSaveOption, inUrl);
}

// ------------------------------------------------------------------------------------------
void
Window::ActivateObject()
{
	OSStatus	err;
	
	err = ::ActivateWindow(*this, true);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Window::MakeSpecifier(
	AEWriter&		ioWriter) const
{
	String	name(GetName());
	
	if (!name.empty())
	{
		BuildNameSpecifier(GetContainer(), GetClassID(), name, ioWriter);
	}
	else
	{
		AEObject::MakeSpecifier(ioWriter);
	}
}

// ------------------------------------------------------------------------------------------
SInt32
Window::CountWindowsOfClass(
	DescType		inClassID)
{
	SInt32	count	= 0;
	
	for (WindowRef windowRef = GetWindowList();
		 windowRef != NULL; 
		 windowRef = GetNextWindow(windowRef))
	{
		Window*	window	= Window::FromWindowRef(windowRef);
		
		if (window == NULL)
			continue;
		
		if (window->InheritsFrom(inClassID))
			count++;
	}
	
	return (count);
}

// ------------------------------------------------------------------------------------------
Window*
Window::GetWindowOfClassByIndex(
	DescType		inClassID, 
	unsigned		inIndex)
{
	Window*	foundWindow	= NULL;
	size_t	i			= 0;
	
	for (WindowRef windowRef = GetWindowList();
		 windowRef != NULL; 
		 windowRef = GetNextWindow(windowRef))
	{
		Window*	window	= Window::FromWindowRef(windowRef);
		
		if (window == NULL)
			continue;
		
		if (!window->InheritsFrom(inClassID))
			continue;
		
		if (i++ == inIndex)
		{
			foundWindow = window;
			break;
		}
	}
	
	return (foundWindow);
}

// ------------------------------------------------------------------------------------------
Window*
Window::GetWindowOfClassByName(
	DescType		inClassID, 
	const String&	inName)
{
	Window*	foundWindow	= NULL;
	
	for (WindowRef windowRef = GetWindowList();
		 windowRef != NULL; 
		 windowRef = GetNextWindow(windowRef))
	{
		Window*	window	= Window::FromWindowRef(windowRef);
		
		if (window == NULL)
			continue;
		
		if (!window->InheritsFrom(inClassID))
			continue;
		
		if (AEObjectSupport::CompareStrings(window->GetName(), inName))
		{
			foundWindow = window;
			break;
		}
	}
	
	return (foundWindow);
}

// ------------------------------------------------------------------------------------------
Window*
Window::GetWindowOfClassByUniqueID(
	DescType		inClassID, 
	SInt32			inUniqueID)
{
	Window*	foundWindow	= NULL;
	
	for (WindowRef windowRef = GetWindowList();
		 windowRef != NULL; 
		 windowRef = GetNextWindow(windowRef))
	{
		Window*	window	= Window::FromWindowRef(windowRef);
		
		if (window == NULL)
			continue;
		
		if (!window->InheritsFrom(inClassID))
			continue;
		
		if (window->GetUniqueID() == inUniqueID)
		{
			foundWindow = window;
			break;
		}
	}
	
	return (foundWindow);
}

// ------------------------------------------------------------------------------------------
void
Window::DebugPrint() const
{
	DebugPrinter	printer(1);
	
	printer(HIViewGetRoot(mWindowRef));
}

// ==========================================================================================
//	Window

// ------------------------------------------------------------------------------------------
void
Window::WindowDeleter::operator () (Window* window) const
{
//printf("releasing %p [%d]\n", window, GetWindowRetainCount(window->GetWindowRef()));

	ReleaseWindow(window->GetWindowRef());
}

}	// namespace B
