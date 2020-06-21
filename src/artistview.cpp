#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify *spotify, const QString &artistId, QWidget *parent) : QDockWidget(parent)
{
	auto window = (MainWindow*) parent;

	auto artist = spotify->artist(artistId);
	setFeatures(QDockWidget::DockWidgetClosable);
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(window->get(artist.image), "jpeg");
	auto coverLabel = new QLabel(this);
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));
	layout->addWidget(coverLabel);
	// Format followers
	char prefix = artist.followers > 1000000 ? 'M' : artist.followers > 1000 ? 'k': '\0';
	auto followers = QString("%1%2 follower%3")
		.arg(prefix == 'M'
			? artist.followers / 1000000 : prefix == 'k'
				? artist.followers / 1000 : artist.followers)
		.arg(prefix)
		.arg(artist.followers == 1 ? "" : "s");

	// Artist name title
	auto title = new QHBoxLayout();
	auto follow = new QPushButton(this);
	follow->setIcon(Icon::get("starred-symbolic"));
	follow->setToolTip(QString("Follow artist (%1)").arg(followers));
	follow->setFlat(true);
	title->addWidget(follow);

	auto name = new QLabel(artist.name, this);
	name->setAlignment(Qt::AlignHCenter);
	name->setWordWrap(true);
	auto titleFont = name->font();
	titleFont.setPointSize(24);
	name->setFont(titleFont);
	title->addWidget(name, 1);

	auto popularityText = QString("%1% popularity").arg(artist.popularity);
	auto popularity = new QPushButton(this);
	popularity->setToolTip(popularityText);
	//popularity->setIcon(Icon::get("draw-donut"));
	popularity->setIcon(QIcon(MainWindow::mask(
		Icon::get("draw-donut").pixmap(64, 64),
		MainWindow::MaskShape::Pie,
		QVariant(artist.popularity))));
	popularity->setFlat(true);
	QAbstractButton::connect(popularity, &QAbstractButton::clicked, [popularity, popularityText](bool checked) {
		QToolTip::showText(QCursor::pos(), popularityText);
	});
	title->addWidget(popularity);

	layout->addLayout(title);

	// Genres
	auto genres = new QLabel(artist.genres.join(", "));
	genres->setWordWrap(true);
	layout->addWidget(genres);
	// Tabs
	auto tabs = new QTabWidget(this);
	layout->addWidget(tabs);
	// Top tracks
	auto topTracks = artist.topTracks(*spotify);
	QStringList topTrackIds;
	auto topTracksList = new QListWidget(tabs);
	for (auto &track : topTracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		item->setIcon(QIcon(window->getAlbum(track.image)));
		item->setData(MainWindow::RoleTrackId, track.id);
		item->setData(MainWindow::RoleAlbumId, track.albumId);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}
	QListWidget::connect(topTracksList, &QListWidget::itemClicked, [this, topTrackIds, spotify, window](QListWidgetItem *item) {
		auto result =  spotify->playTracks(
			QString("spotify:track:%1").arg(item->data(MainWindow::RoleTrackId).toString()), topTrackIds);
		if (!result.isEmpty())
			window->setStatus(QString("Failed to start playback: %1").arg(result), true);
	});
	topTracksList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(topTracksList, &QWidget::customContextMenuRequested, [topTracksList, artist, window, spotify](const QPoint &pos) {
		auto item = topTracksList->itemAt(pos);
		auto trackId = item->data(MainWindow::RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		(new SongMenu(trackId, artist.name, item->text(), artist.id,
			item->data(MainWindow::RoleAlbumId).toString(), spotify, window))
			->popup(topTracksList->mapToGlobal(pos));
	});
	tabs->addTab(topTracksList, "Popular");
	// Albums
	auto albums = artist.albums(*spotify);
	auto albumList = new QTreeWidget(tabs);
	auto singleList = new QTreeWidget(tabs);
	for (auto &list : {albumList, singleList})
	{
		list->setColumnCount(2);
		list->header()->hide();
		list->setRootIsDecorated(false);
		list->header()->resizeSection(0, 235);
		list->header()->resizeSection(1, 1);
	}
	for (auto &album : albums)
	{
		auto parentTab = album.albumGroup == "single" ? singleList : albumList;
		auto year = album.releaseDate.toString("yyyy");
		auto item = new QTreeWidgetItem(parentTab, {
			album.name,
			year.isEmpty() ? QString() : year
		});
		item->setIcon(0, QIcon(window->getAlbum(album.image)));
		item->setData(0, MainWindow::RoleAlbumId, album.id);
		parentTab->insertTopLevelItem(0, item);
	}
	auto loadAlbumId = [this, window](QTreeWidgetItem *item) {
		if (!window->loadAlbum(item->data(0, MainWindow::RoleAlbumId).toString(), false))
			window->setStatus(QString("Failed to load album"), true);
	};
	QTreeWidget::connect(albumList,  &QTreeWidget::itemClicked, loadAlbumId);
	QTreeWidget::connect(singleList, &QTreeWidget::itemClicked, loadAlbumId);
	tabs->addTab(albumList,  "Albums");
	tabs->addTab(singleList, "Singles");
	// Related artists
	auto relatedArtists = artist.relatedArtists(*spotify);
	auto relatedList = new QListWidget(tabs);
	for (auto &related : relatedArtists)
	{
		auto item = new QListWidgetItem(related.name, relatedList);
		item->setData(MainWindow::RoleArtistId, related.id);
	}
	QListWidget::connect(relatedList, &QListWidget::itemClicked, [this, relatedList, window](QListWidgetItem *item) {
		relatedList->setEnabled(false);
		window->openArtist(item->data(MainWindow::RoleArtistId).toString());
	});
	tabs->addTab(relatedList, "Related");
	// Rest of dock
	setWidget(MainWindow::layoutToWidget(layout));
	setFixedWidth(320);
}