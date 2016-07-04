// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// This file contains various instantiations of the template classes in the B framework.

// B headers
#include "BApplication.h"
#include "BDocument.h"
#include "BMultipleDocumentFactory.h"
#include "BMultipleDocumentPolicy.h"
#include "BMultipleUndoPolicy.h"
#include "BNullDocumentFactory.h"
#include "BNullDocumentPolicy.h"
#include "BNullUndoPolicy.h"
#include "BNullPrintPolicy.h"
#include "BPrefsPrintPolicy.h"
#include "BSingleDocumentFactory.h"
#include "BSingleUndoPolicy.h"

// template instantiations
#include "BApplication.tpl.h"
#include "BDocument.tpl.h"
#include "BDocumentWindow.tpl.h"
#include "BMultipleDocumentPolicy.tpl.h"


// Document
template class  B::Document<
                    B::NullUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Document<
                    B::NullUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Document<
                    B::SingleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Document<
                    B::SingleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Document<
                    B::MultipleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Document<
                    B::MultipleUndoPolicy,
                    B::PrefsPrintPolicy>;

// Document Window
template class  B::DocumentWindow<
                    B::Document<
                        B::NullUndoPolicy,
                        B::NullPrintPolicy> >;
template class  B::DocumentWindow<
                    B::Document<
                        B::NullUndoPolicy,
                        B::PrefsPrintPolicy> >;
template class  B::DocumentWindow<
                    B::Document<
                        B::SingleUndoPolicy,
                        B::NullPrintPolicy> >;
template class  B::DocumentWindow<
                    B::Document<
                        B::SingleUndoPolicy,
                        B::PrefsPrintPolicy> >;
template class  B::DocumentWindow<
                    B::Document<
                        B::MultipleUndoPolicy,
                        B::NullPrintPolicy> >;
template class  B::DocumentWindow<
                    B::Document<
                        B::MultipleUndoPolicy,
                        B::PrefsPrintPolicy> >;

// Document Factory
template class  B::SingleDocumentFactory<
                    B::Document<
                        B::NullUndoPolicy,
                        B::NullPrintPolicy> >;

// Document Policy
template class  B::MultipleDocumentPolicy<
                    B::NullDocumentFactory>;
template class  B::MultipleDocumentPolicy<
                    B::MultipleDocumentFactory>;
template class  B::MultipleDocumentPolicy<
                    B::SingleDocumentFactory<
                        B::Document<
                            B::NullUndoPolicy,
                            B::NullPrintPolicy> > >;

// Application
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::NullUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::NullUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::SingleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::SingleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::MultipleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::NullDocumentPolicy,
                    B::MultipleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::NullUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::NullUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::SingleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::SingleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::MultipleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::NullDocumentFactory>,
                    B::MultipleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::NullUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::NullUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::SingleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::SingleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::MultipleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::MultipleDocumentFactory>,
                    B::MultipleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::NullUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::NullUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::SingleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::SingleUndoPolicy,
                    B::PrefsPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::MultipleUndoPolicy,
                    B::NullPrintPolicy>;
template class  B::Application<
                    B::MultipleDocumentPolicy<
                        B::SingleDocumentFactory<
                            B::Document<
                                B::NullUndoPolicy,
                                B::NullPrintPolicy> > >,
                    B::MultipleUndoPolicy,
                    B::PrefsPrintPolicy>;


int main()
{
}
