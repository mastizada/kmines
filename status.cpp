#include "status.h"
#include "status.moc"

#include <qpainter.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qobjectlist.h>
#include <qpicture.h>

#include <kapp.h>
#include <kconfig.h>

#include "defines.h"
#include "field.h"
#include "dialogs.h"


KMinesStatus::KMinesStatus(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	frame = new QFrame(this);
	frame->setFrameStyle( QFrame::Box | QFrame::Raised );
	frame->setLineWidth(2);
	frame->setMidLineWidth(2);
	
	field = new Field(this);
  
	connect( field, SIGNAL(changeCase(uint,uint)),
			 SLOT(changeCase(uint,uint)) );
	connect( field, SIGNAL(updateStatus(bool)), SLOT(update(bool)) );
	connect( field, SIGNAL(endGame(int)), SLOT(endGame(int)) );
	connect( parent, SIGNAL(UMarkChanged(bool)),
			 field, SLOT(changeUMark(bool)) );
  
	smiley = new QPushButton(this);
	connect( smiley, SIGNAL(clicked()), SLOT(restartGame()) );
	smiley->installEventFilter(parent);
  
	QPainter pt;
	
	s_ok = new QPixmap(25,25);
	createSmileyPixmap(s_ok,&pt);
	pt.drawPoint(8,14); pt.drawPoint(16,14);
	pt.drawLine(9,15,15,15);
	pt.end();
	s_stress = new QPixmap(25,25);
	createSmileyPixmap(s_stress,&pt);
	pt.drawPoint(12,13);
	pt.drawLine(11,14,11,15); pt.drawLine(13,14,13,15);
	pt.drawPoint(12,16);
	pt.end();
	s_happy = new QPixmap(25,25);
	createSmileyPixmap(s_happy,&pt);
	pt.drawPoint(7,14); pt.drawPoint(17,14);
	pt.drawPoint(8,15); pt.drawPoint(16,15);
	pt.drawPoint(9,16); pt.drawPoint(15,16);
	pt.drawLine(10,17,14,17);
	pt.end();
	s_ohno = new QPixmap(25,25);
	createSmileyPixmap(s_ohno,&pt);  pt.drawPoint(12,11);
	pt.drawLine(10,13,14,13);
	pt.drawLine(9,14,9,17); pt.drawLine(15,14,15,17);
	pt.drawLine(10,18,14,18);
	pt.end();
  
	dg = new DigitalClock(this);
	dg->installEventFilter(parent);
	
	left = new QLCDNumber(this);
	left->setFrameStyle(  QFrame::Panel | QFrame::Sunken );
	left->installEventFilter(parent);
	
	setFont( QFont("Times", 14, QFont::Bold) );
	
	mesg = new QLabel(this);
	mesg->setAlignment( AlignCenter );
	mesg->setFrameStyle(  QFrame::Panel | QFrame::Sunken );
	mesg->installEventFilter(parent);
	
	connect( this, SIGNAL(exleft(const QString &)),
			 left, SLOT(display(const QString &)) );

	connect( this, SIGNAL(freezeTimer()), dg, SLOT(freeze()) );
	connect( this, SIGNAL(getTime(int &, int &)),
		 	 dg,   SLOT(getTime(int &, int &)) );
	
	connect( field, SIGNAL(startTimer()), dg, SLOT(start()) );
	connect( field, SIGNAL(freezeTimer()), dg, SLOT(freeze()) );
	connect( this,  SIGNAL(zeroTimer()), dg, SLOT(zero()) );
	connect( this,  SIGNAL(startTimer()), dg, SLOT(start()) );
	connect( field, SIGNAL(updateSmiley(int)), this, SLOT(updateSmiley(int)) );
	
	/* configuration & highscore initialisation */
	KConfig *kconf = kapp->getConfig();
	for (int i=0; i<3; i++) {
		kconf->setGroup(HS_GRP[i]);
		if ( !kconf->hasKey(HS_NAME_KEY) )
			kconf->writeEntry(HS_NAME_KEY, i18n("Anonymous"));
		if ( !kconf->hasKey(HS_MIN_KEY) )
			kconf->writeEntry(HS_MIN_KEY, 59);
		if ( !kconf->hasKey(HS_SEC_KEY) )
			kconf->writeEntry(HS_SEC_KEY, 59);    
	}
}

void KMinesStatus::createSmileyPixmap(QPixmap *pm, QPainter *pt)
{
	pm->fill(yellow);
	pt->begin(pm);
	pt->setPen(black);
	pt->drawLine(9,3,15,3);
	pt->drawLine(7,4,8,4); pt->drawLine(16,4,17,4);
	pt->drawPoint(6,5); pt->drawPoint(18,5);
	pt->drawPoint(5,6); pt->drawPoint(19,6);
	pt->drawLine(4,7,4,8); pt->drawLine(20,7,20,8);
	pt->drawLine(8,7,9,7); pt->drawLine(15,7,16,7);
	pt->drawLine(3,9,3,14); pt->drawLine(21,9,21,14);
	pt->drawPoint(12,10);
	pt->drawLine(4,15,4,17); pt->drawLine(20,15,20,17);
	pt->drawPoint(5,18); pt->drawPoint(19,18);
	pt->drawLine(6,19,7,19); pt->drawLine(17,19,18,19);
	pt->drawLine(8,20,9,20); pt->drawLine(15,20,16,20);
	pt->drawLine(10,21,14,21);
}

void KMinesStatus::adjustSize()
{ 
	int dec_w  = (width() - 2*LCD_W - SMILEY_W)/4;
	int dec_h  = (STAT_H - LCD_H)/2;
	int dec_hs = (STAT_H - SMILEY_H)/2;
	
	left->setGeometry( dec_w, dec_h, LCD_W, LCD_H );
	smiley->setGeometry( 2*dec_w + LCD_W, dec_hs, SMILEY_W, SMILEY_H );
	dg->setGeometry( 3*dec_w + LCD_W + SMILEY_W, dec_h, LCD_W, LCD_H );

	
	dec_w = (width() - 2*FRAME_W - nb_width*CASE_W)/2;
	dec_h = (height() - STAT_H - LABEL_H - 2*FRAME_W - nb_height*CASE_W)/2;
	
	frame->setGeometry( dec_w, STAT_H + dec_h + LABEL_H,
					    nb_width*CASE_W + 2*FRAME_W,
					    nb_height*CASE_W + 2*FRAME_W );
	field->setGeometry( FRAME_W + dec_w, STAT_H + FRAME_W + dec_h + LABEL_H,
					    nb_width*CASE_W, nb_height*CASE_W );
}

void KMinesStatus::restartGame()
{
	uncovered = 0; uncertain = 0; marked = 0;
	
	/* hide the message label */
	mesg->hide();
	
	update(FALSE);
	updateSmiley(OK);
	
	emit zeroTimer();
	emit newField( nb_width, nb_height, nb_mines);
}

void KMinesStatus::newGame(uint nb_w, uint nb_h, uint nb_m)
{
	nb_width  = nb_w;
	nb_height = nb_h;
	nb_mines  = nb_m;
	
	adjustSize();
	restartGame();
}

void KMinesStatus::changeCase(uint case_mode, uint inc)
{
	switch(case_mode) {
	 case UNCOVERED : uncovered += inc; break;
	 case UNCERTAIN : uncertain += inc; break;
	 case MARKED    : marked    += inc; break;
	}
}

void KMinesStatus::update(bool mine)
{
	QString str;
	str.setNum(nb_mines - marked);
	emit exleft(str);
	
	if ( uncovered==(nb_width*nb_height - nb_mines) ) endGame(!mine);
}

void KMinesStatus::updateSmiley(int mood)
{
	switch (mood) {
	 case OK      : smiley->setPixmap(*s_ok); break;
	 case STRESS  : smiley->setPixmap(*s_stress); break;
	 case HAPPY   : smiley->setPixmap(*s_happy); break;
	 case UNHAPPY : smiley->setPixmap(*s_ohno); break;
	}
}

void KMinesStatus::endGame(int win)
{
	int t_sec, t_min, res = 0;
	
	emit stopField();
	emit freezeTimer();
	
	if (win) {
		emit updateSmiley(HAPPY);
		emit getTime(t_sec, t_min);
		
		if (nb_width==MODES[0][0] && nb_height==MODES[0][1] && nb_mines==MODES[0][2])
			res = setHighScore(t_sec, t_min, EASY);
		else if (nb_width==MODES[1][0] && nb_height==MODES[1][1] && nb_mines==MODES[1][2])
			res = setHighScore(t_sec, t_min, NORMAL);
		else if (nb_width==MODES[2][0] && nb_height==MODES[2][1] && nb_mines==MODES[2][2])
			res = setHighScore(t_sec, t_min, EXPERT);
		
		if ( res!=0 ) exmesg(i18n("You did it ... but not in time."));
		else exmesg(i18n("Yeeessss !"));
	} else {
		emit updateSmiley(UNHAPPY);
		exmesg(i18n("Bad luck !"));
	}
}

void KMinesStatus::getNumbers(uint &width, uint &height, uint &nbMines)
{
	width   = nb_width;
	height  = nb_height;
	nbMines = nb_mines;
}

void KMinesStatus::exmesg(const QString &str)
{ 
	QFontMetrics fm1( font() );
	int w = fm1.width(str)+10;
	mesg->setGeometry((width()-w)/2, STAT_H, w, fm1.height()+5);
	mesg->show();
	mesg->setText(str);
}

void KMinesStatus::showHighScores()
{
	int dummy;
	WHighScores whs(TRUE, 0, 0, 0, dummy, this);
}

int KMinesStatus::setHighScore(int sec, int min, int mode)
{
	int res = 0;
	WHighScores whs(FALSE, sec, min, mode, res, this);
	return res;
} 

void KMinesStatus::print()
{
	QPrinter prt;
	if ( !prt.setup() ) return;

	// repaint all children widgets
	repaint(FALSE);
	const QObjectList *ol = children();
	QObjectListIt it(*ol);
	QObject *o;
	QWidget *w;
	while ( (o=it.current()) ) {
		++it;
		if ( !o->isWidgetType()) continue;
		w = (QWidget *)o;
		w->repaint(FALSE);
	}

	// write the screen region corresponding to the window
	QPainter p(&prt);
	p.drawPixmap(0, 0, QPixmap::grabWindow(winId())); 

//  QRect r = p.viewport();
}



