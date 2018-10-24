/***************************************************************************
 * 
 * $Id$
 * 
 **************************************************************************/

/**
 * @file $HeadURL$
 * @author $Author$(hoping@baimashi.com)
 * @date $Date$
 * @version $Revision$
 * @brief 
 *  
 **/

#ifndef SELECTION_H_
#define SELECTION_H_

#include "Object.h"
#include <vector>
#include <string>
#include <gumbo.h>
#include "Core/Scripting/Squirrelnc.h"

class CNode;

class CSelection: public CObject
{

	public:
        CSelection();
		CSelection(GumboNode* apNode);

		CSelection(std::vector<GumboNode*> aNodes);
        std::string attribute(std::string key);

        std::string text();
        std::string tagName();
        std::string ownText();


		virtual ~CSelection();
        CSelection& each(std::function<void(int, CNode)> callback);
        CSelection& squirrelEach(Sqrat::Function callback);
	public:

		CSelection find(std::string aSelector);

		CNode nodeAt(unsigned int i);

		unsigned int nodeNum();

	private:

		std::vector<GumboNode*> mNodes;
};

#endif /* SELECTION_H_ */

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
