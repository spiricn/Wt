
template<class T>
T* ARsrcManagerTab::createResource(RTWItem* item, const QString& rsrcName){
	wt::AResourceGroup<T>* parent;

	if(item==NULL){
		parent = static_cast<wt::AResourceManager<T>*>(mManager);
	}
	else{
		parent = static_cast<wt::AResourceGroup<T>*>(item->userData);
	}

		
	QString name;
	if(rsrcName.isEmpty()){
		bool ok;
		name = QInputDialog::getText(mParent, "Input",
					"Resource name: ", QLineEdit::Normal, "", &ok);
		if(!ok || name.isEmpty()){
			return NULL;
		}
	}

	T* rsrc = parent->create(name.toStdString());
	mTree->createItem(item, RTWItem::RESOURCE, name, rsrc);

	return rsrc;
}

template<class T>
void ARsrcManagerTab::deleteItem(RTWItem* item, bool askConfirm){
	int res = QMessageBox::question(
		mParent, "Confirmation", "Delete resource?", QMessageBox::Yes, QMessageBox::No);

	if(res != QMessageBox::Yes){
		return;
	}

	wt::AResourceGroup<T>* parentGroup = item->parent==NULL ? (wt::AResourceManager<T>*)mManager : (wt::AResourceGroup<T>*)item->parent->userData;

	if(item->type == RTWItem::GROUP){	
		parentGroup->deleteChild( ((wt::AResourceGroup<T>*)item->userData)->getName() );
	}
	else{
		parentGroup->deleteResource( ((wt::AResourceGroup<T>*)item->userData)->getName() );
	}

	mTree->deleteItem(item);
}

template<class T>
void ARsrcManagerTab::createResourceGroup(RTWItem* item){
	wt::AResourceGroup<T>* parent;

	if(item==NULL){
		parent = static_cast<wt::AResourceManager<T>*>(mManager);
	}
	else{
		parent = static_cast<wt::AResourceGroup<T>*>(item->userData);
	}

	bool ok;
	QString name = QInputDialog::getText(mParent, "Input",
		"Resource group name: ", QLineEdit::Normal, "", &ok);
	if(ok){
		wt::AResourceGroup<T>* group = parent->createGroup(name.toStdString());
		mTree->createItem(item, RTWItem::GROUP, name, group);
	}
}

template<class T>
void ARsrcManagerTab::initialize(ResourceTreeWidget* treeWidget){
	setTreeWidget(treeWidget);

	mImpl = createResourceImplementation<T>(this);
}