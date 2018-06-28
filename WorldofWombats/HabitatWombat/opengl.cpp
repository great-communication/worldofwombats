#include "opengl.h"
#include <QMouseEvent>
#include "gui.h"
#include "Widgets/Game/console.h"

OpenGL::OpenGL(QWidget *parent)
    : QOpenGLWidget(parent),
      clearColor(Qt::black),
      program(0)
{    
    selectedWombatVisibility = true;
}

OpenGL::~OpenGL()
{
    makeCurrent();
    vbo.destroy();

    //delete texture;
    delete program;
    doneCurrent();
}

void OpenGL::updateBackgroundSize(QSize backgroundSize){
    this->backgroundSize = backgroundSize;
    xFct = (2.0f/(float)backgroundSize.width()); //Converting pixels to openGL coords
    yFct = (2.0f/(float)backgroundSize.height());
}

void OpenGL::updateBackgroundTextureSize(const QImage &backgroundImage){
    glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, backgroundSize.width(), backgroundSize.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backgroundImage.bits());
    glBindTexture( GL_TEXTURE_2D, 0);
    float x = (float)((float)this->width()/backgroundSize.width());
    float y = (float)((float)this->height()/backgroundSize.height());
    //qDebug()<<"width: "<<x;
    //qDebug()<<"height: "<<y;
    worldProjection.setToIdentity();
    worldProjection.ortho(-x, +x, +y, -y, 4.0f, 15.0f);
    worldProjection.translate(0.0f, 0.0f, -10.0f);
}

void OpenGL::setup(QSize backgroundSize){
    this->backgroundSize = backgroundSize;
    xFct = (2.0f/(float)backgroundSize.width()); //Converting pixels to openGL coords
    yFct = (2.0f/(float)backgroundSize.height());   
    //updateBackgroundSize(backgroundSize);
    originalWindowSize = QPointF(gui->windowSize.width(),gui->windowSize.height());   
    //xFct = yFct = ((2.0f/4096.0f)); //Converting pixels to openGL coords
    fuckyoucunt = 0;
    fuckSoMuch.start();
    fuckSoMuch72.start();
    bestFuckSoFar = 0;
}

QSize OpenGL::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGL::sizeHint() const
{
    return QSize(4096, 4096);
}

void OpenGL::initializeGL()
{
    initializeOpenGLFunctions();

    makeObject();

    glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
            //glTexImage2D(GL_TEXTURE_2D, 0, 3, 4096,  4096, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, backgroundSize.width(),  backgroundSize.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture( GL_TEXTURE_2D, 0);

    QImage wombatImage = QImage(":/HabitatWombat/Images/HabitatWombat/Images/wombat.png").mirrored().rgbSwapped();
    glGenTextures(1, &wombatTex);
        glBindTexture(GL_TEXTURE_2D, wombatTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wombatImage.width(), wombatImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, wombatImage.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture( GL_TEXTURE_2D, 0);

    QImage wombatSleepingImage = QImage(":/HabitatWombat/Images/HabitatWombat/Images/wombat_sleeping.png").mirrored().rgbSwapped();
    glGenTextures(1, &wombatSleepingTex);
        glBindTexture(GL_TEXTURE_2D, wombatSleepingTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wombatSleepingImage.width(), wombatSleepingImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, wombatSleepingImage.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture( GL_TEXTURE_2D, 0);

    QImage exclamationImage = QImage(":/HabitatWombat/Images/Emoticons/HabitatWombat/Images/Emoticons/exclamation.png").mirrored().rgbSwapped();
    glGenTextures(1, &exclamationEmoticon);
        glBindTexture(GL_TEXTURE_2D, exclamationEmoticon);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, exclamationImage.width(), exclamationImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, exclamationImage.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture( GL_TEXTURE_2D, 0);

    QImage sleepingImage = QImage(":/HabitatWombat/Images/Emoticons/HabitatWombat/Images/Emoticons/sleeping.png").mirrored().rgbSwapped();
    glGenTextures(1, &sleepingEmoticon);
        glBindTexture(GL_TEXTURE_2D, sleepingEmoticon);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sleepingImage.width(), sleepingImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, sleepingImage.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture( GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);

    //worldProjection.ortho(-0.390625f, +0.390625f, +0.2197265625f, -0.2197265625f, 4.0f, 15.0f);
    float x = (float)(originalWindowSize.x()/backgroundSize.width());
    float y = (float)(originalWindowSize.y()/backgroundSize.height());

    worldProjection.ortho(-x, +x, +y, -y, 4.0f, 15.0f);
    worldProjection.translate(0.0f, 0.0f, -10.0f);

    x = (float)(originalWindowSize.x()/wombatImage.width());
    y = (float)(originalWindowSize.y()/wombatImage.height());

    wombatProjection.ortho(-x, +x, +y, -y, 4.0f, 14.0f); // 12.5 = 1600/128
    wombatProjection.translate(0.0f, 0.0f, -9.0f);
}

void OpenGL::updateTexturePart(const QImage &backgroundImage,QPoint visibleTilePos){
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, visibleTilePos.x(), visibleTilePos.y(), backgroundImage.width(), backgroundImage.height(), GL_RGBA, GL_UNSIGNED_BYTE, backgroundImage.bits());
    glBindTexture( GL_TEXTURE_2D, 0);
}

void OpenGL::updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground){
    backgroundImageUnderground = underground;
    //if(selectedWombatVisibility){
        if(wombatsOnDisplay.length()>0){
            wombatsOnDisplay[0].underground = underground;
        }
    //}

    if(backgroundSize.width()!= backgroundImage.width() || backgroundSize.height() != backgroundImage.height()){
        updateBackgroundSize(QSize(backgroundImage.width(),backgroundImage.height()));
        updateBackgroundTextureSize(backgroundImage);
        //qDebug()<<"openGL: "<<backgroundImage.width();
        //qDebug()<<"openGL: "<<backgroundImage.height();
    }else{
        glBindTexture(GL_TEXTURE_2D, tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, backgroundImage.width(), backgroundImage.height(), GL_RGBA, GL_UNSIGNED_BYTE, backgroundImage.bits());
        glBindTexture( GL_TEXTURE_2D, 0);
    }

    int x = positionsChanged.x()*gTileSize;
    int y = positionsChanged.y()*gTileSize;

    gui->xMoved2 -= x;
    gui->yMoved2 -= y;
    updateWorldTranslate();
    gui->updatingTiles = false;
    //console->print("Update complete");

    QPoint addOffset = QPoint(-x,-y);
    //console->print("offset "+QString::number(addOffset.x())+","+QString::number(addOffset.y()));
    for (int i = 0; i < wombatsOnDisplay.length(); ++i) {
        wombatsOnDisplay[i].originalOffset += addOffset;
    }
}

void OpenGL::resetOriginalOffset(){
    for (int i = 0; i < wombatsOnDisplay.length(); ++i) {
        wombatsOnDisplay[i].originalOffset = QPoint(0,0);
    }
}

void OpenGL::updateWorldTranslate(){
    worldTranslate.setToIdentity();      
    worldTranslate.translate((xFct*-(float)gui->xMoved2), (yFct*-(float)gui->yMoved2), 0.0f);
}

void OpenGL::updateWorldTranslate2(){
    float x = (float)((float)this->width()/backgroundSize.width());
    float y = (float)((float)this->height()/backgroundSize.height());
    worldProjection.setToIdentity();
    worldProjection.ortho(-x, +x, +y, -y, 4.0f, 15.0f);
    worldProjection.translate(0.0f, 0.0f, -10.0f);
}

void OpenGL::paintGL(){
  /*  {
    int msSinceLast = fuckSoMuch72.restart();
        if(msSinceLast>60){

            console->print("start paintGL, elapsed time since last end: "+QString::number(msSinceLast)+" ms **");
        }
    }
*/
    unloadNextPosFromWombatPosBuffer();

    if(gui->moving){
        gui->move();
        updateWorldTranslate();
    }else if(gui->scrolling){ //Is else if ok? Or should it be a separate if statement?
        gui->scrollToTargetPos();
        updateWorldTranslate();
    }

    //glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //program->setUniformValue("matrix", worldProjection*worldTranslate*projectionOffset);
    program->setUniformValue("matrix", worldProjection*worldTranslate);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    //Draw background
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);    

    //Draw wombats
    //First draw all sleeping wombats
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, wombatSleepingTex);

    QVector<int> awake;
    QVector<int> labelSleeping;
    QVector<int> labelExclamation;
    QVector<int> ageFUCKTEMPFIXSleep;
    QVector<int> ageFUCKTEMPFIXExclamation;
    for (int i = wombatsOnDisplay.length()-1; i >= 0; i--) { //Have to do it from the back because wombat[0] needs to be drawn last so that it is ontop of all other wombats
        if (wombatsOnDisplay[i].underground == backgroundImageUnderground){
            if(!(i==0 && selectedWombatVisibility==false)){ // i=0 is the selected wombat
                if(wombatsOnDisplay[i].currentTask == WombatAITask_Sleeping){
                    drawWombat(i,0);
                }else{
                    awake.append(i);
                }

                if(wombatsOnDisplay[i].emoticon != WombatEmoticon_None){
                    if(wombatsOnDisplay[i].emoticon == WombatEmoticon_Sleeping){
                        labelSleeping.append(i);
                        ageFUCKTEMPFIXSleep.append(wombatsOnDisplay[i].age);
                    }else if (wombatsOnDisplay[i].emoticon == WombatEmoticon_Exclamation){
                        labelExclamation.append(i);
                        ageFUCKTEMPFIXExclamation.append(wombatsOnDisplay[i].age);
                    }
                }
            }
        }
    }

    //Then draw all awake wombats
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, wombatTex);

    for (int i = 0; i<awake.length(); i++) {
        drawWombat(awake[i],0.5f);
    }

    //Draw all sleepingLabels
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, sleepingEmoticon);
    for (int i = 0; i<labelSleeping.length(); i++) {
        drawLabel(labelSleeping[i],ageFUCKTEMPFIXSleep.value(i));
    }

    //Draw all exclamationLabels
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, exclamationEmoticon);
    for (int i = 0; i<labelExclamation.length(); i++) {
        drawLabel(labelExclamation[i],ageFUCKTEMPFIXExclamation.value(i));
    }

/*
    {
    int msSinceLast = fuckSoMuch72.restart();
        if(msSinceLast>5){
            if(msSinceLast>bestFuckSoFar){
                bestFuckSoFar = msSinceLast;
            }
            console->print("end paintGL, elapsed time: "+QString::number(msSinceLast)+" ms ---");
        }
    }*/
}

void OpenGL::drawLabel(int i, int age){
    wombatTranslate.setToIdentity();

    if (age>1){
        wombatTranslate.translate(((wombatsOnDisplay[i].glPos.x()/(2.0f/128.0f))*(2.0f/32.0f)),((wombatsOnDisplay[i].glPos.y()/(2.0f/128.0f))*(2.0f/32.0f)),0.0f);
    }else if (age==1){
        wombatTranslate.translate(((wombatsOnDisplay[i].glPos.x()/(2.0f/104.0f))*(2.0f/32.0f)),((wombatsOnDisplay[i].glPos.y()/(2.0f/104.0f))*(2.0f/32.0f)),0.0f);
    }else{
        wombatTranslate.translate(((wombatsOnDisplay[i].glPos.x()/(2.0f/80.0f))*(2.0f/32.0f)),((wombatsOnDisplay[i].glPos.y()/(2.0f/80.0f))*(2.0f/32.0f)),0.0f);
    }

    QMatrix4x4 translateOffset;
    float depth = i;
    depth = depth/1000;
    depth = -8.0f - depth;

    float xx = (float)((float)this->width()/(float)32);
    float yy = (float)((float)this->height()/(float)32);
    wombatProjection.setToIdentity();
    wombatProjection.ortho(-xx, +xx, +yy, -yy, 4.0f, 14.0f); // 12.5 = 1600/32
    wombatProjection.translate(0.0f, 0.0f, depth);
    int x = gui->xMoved2-wombatsOnDisplay[i].originalOffset.x();
    int y = gui->yMoved2-wombatsOnDisplay[i].originalOffset.y();
    translateOffset.translate((2.0f/32.0f)*-(float)x,(2.0f/32.0f)*-(float)y, 0.0f);
    //program->setUniformValue("matrix", wombatProjection*wombatTranslate*translateOffset);
    program->setUniformValue("matrix", wombatProjection*wombatTranslate*translateOffset);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OpenGL::drawWombat(int i, float depthAdd){
    wombatTranslate.setToIdentity();
    wombatTranslate.translate(wombatsOnDisplay[i].glPos.x(),wombatsOnDisplay[i].glPos.y(),0.0f);
    QMatrix4x4 translateOffset;
    wombatRotation.setToIdentity();
    wombatRotation.rotate((float)wombatsOnDisplay[i].rotation,0.0f,0.0f,1.0f);
    float depth = i;
    depth = depth/1000;
    depth = -9.0f - depth + depthAdd;
    if (wombatsOnDisplay[i].age>1){
        float xx = (float)((float)this->width()/(float)128);
        float yy = (float)((float)this->height()/(float)128);
        wombatProjection.setToIdentity();
        wombatProjection.ortho(-xx, +xx, +yy, -yy, 4.0f, 14.0f); // 12.5 = 1600/128
        wombatProjection.translate(0.0f, 0.0f, depth);
        int x = gui->xMoved2-wombatsOnDisplay[i].originalOffset.x();
        int y = gui->yMoved2-wombatsOnDisplay[i].originalOffset.y();
        translateOffset.translate((2.0f/128.0f)*-(float)x,(2.0f/128.0f)*-(float)y, 0.0f);
    }else if (wombatsOnDisplay[i].age==1){
        float xx = (float)((float)this->width()/(float)104);
        float yy = (float)((float)this->height()/(float)104);
        wombatProjection.setToIdentity();
        wombatProjection.ortho(-xx, +xx, +yy, -yy, 4.0f, 14.0f); //wombat size 104px
        wombatProjection.translate(0.0f,0.0f,depth);
        int x = gui->xMoved2-wombatsOnDisplay[i].originalOffset.x();
        int y = gui->yMoved2-wombatsOnDisplay[i].originalOffset.y();
        translateOffset.translate((2.0f/104.0f)*-(float)x,(2.0f/104.0f)*-(float)y, 0.0f);
    }else{
        float xx = (float)((float)this->width()/(float)80);
        float yy = (float)((float)this->height()/(float)80);
        wombatProjection.setToIdentity();
        wombatProjection.ortho(-xx, +xx, +yy, -yy, 4.0f, 14.0f); //wombat size 80px
        wombatProjection.translate(0.0f,0.0f,depth);
        int x = gui->xMoved2-wombatsOnDisplay[i].originalOffset.x();
        int y = gui->yMoved2-wombatsOnDisplay[i].originalOffset.y();
        translateOffset.translate((2.0f/80.0f)*-(float)x,(2.0f/80.0f)*-(float)y, 0.0f);
    }

    if (i==0){ //Selected wombat
        program->setUniformValue("matrix", wombatProjection*wombatTranslate*wombatRotation);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }else{
        program->setUniformValue("matrix", wombatProjection*wombatTranslate*translateOffset*wombatRotation);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

}

void OpenGL::resizeGL(int width, int height){
    updateWorldTranslate2();

    /*
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    */
}

void OpenGL::makeObject(){
    static const int coords[1][4][3] = {
        { { 1, -1, -1 }, { -1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 } }
    };

    QVector<GLfloat> vertData;

        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[0][j][0]);
            vertData.append(coords[0][j][1]);
            vertData.append(coords[0][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
/* last fuck same as:
 *      vertData.append(1);
        vertData.append(1);

        vertData.append(0);
        vertData.append(1);

        vertData.append(0);
        vertData.append(0);

        vertData.append(1);
        vertData.append(0);
*/

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}



void OpenGL::addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask){
    //console->print("Adding wombat to display id: "+QString::number(wombatId));

    //console->print("Wombat added");
    //console->print("openGL added wombat to display");
    //console->print("numberofwombatsondisplay"+QString::number(wombatsOnDisplay.length()));
    QPoint tileOffset = tilePos-gui->viewPosition; //Wombat tile offset from ViewPosition
    QPoint tileOffsetInPixels = QPoint(tileOffset.x()*128,tileOffset.y()*128); //Wombat tile offset from ViewPosition in pixels
    QPoint internalOffsetInPixels = QPoint(gui->xMoved,gui->yMoved); //Positioning within the tile
    QPoint offset = tileOffsetInPixels-internalOffsetInPixels+startingOffsetPx; //Position wombat in middle of tile

    //console->print("tilepos "+QString::number(tilePos.x())+","+QString::number(tilePos.y()));
    //console->print("viewpos "+QString::number(gui->viewPosition.x())+","+QString::number(gui->viewPosition.y()));
    //console->print("tileoffset "+QString::number(tileOffset.x())+","+QString::number(tileOffset.y()));
    //console->print("tileoffsetinpixels"+QString::number(tileOffsetInPixels.x())+","+QString::number(tileOffsetInPixels.y()));
    //console->print("internaloffsetinpixels"+QString::number(internalOffsetInPixels.x())+","+QString::number(internalOffsetInPixels.y()));
    //console->print("offset"+QString::number(offset.x())+","+QString::number(offset.y()));

    //Convert pixels to GL pos
    glWombatStruct wombat;
    wombat.wombatId = wombatId;
    float factor;
    if (age>1){
        factor = (2.0f/128.0f); //Grown wombat size
    }else if (age==1){
        factor = (2.0f/104.0f);
    }else{
        factor = (2.0f/80.0f); // Joey wombat size
    }

    wombat.glPos = QPointF((float)(offset.x() * factor), (float)(offset.y()*factor));
    //if (resetTranslate){
    //    wombat.originalOffset = QPoint (0,0);   //Reset
    //}else{
        wombat.originalOffset = QPoint (gui->xMoved2,gui->yMoved2);   //Wombat original offset from WorldPosition in pixels
    //}
    wombat.rotation = rotation;
    wombat.underground = underground;
    wombat.age = age;
    wombat.currentTask = currentTask;
    wombat.emoticon = WombatEmoticon_None;
    wombat.moving = false;
    wombat.startTimerCalled = false;
    wombatsOnDisplay.append(wombat);
    //console->print("glPos"+QString::number(wombat.glPos.x())+","+QString::number(wombat.glPos.y()));
    //console->print("original offset"+QString::number(wombat.originalOffset.x())+","+QString::number(wombat.originalOffset.y()));
    //hej->append("moved total"+QString::number(movedXTotal)+","+QString::number(movedYTotal));
    //console->print("openGL added wombat to display   age:"+QString::number(wombat.age)+" underground: "+QString::number(wombat.underground));
    //console->print("numberofwombatsondisplay"+QString::number(wombatsOnDisplay.length()));
}

void OpenGL::addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age){
    int index = getWombatOnDisplayIndex(wombatId);

    if (index!=None){
        wombatPosBufferStruct nextPos;
        nextPos.displayIndex = index;
        nextPos.pxChange = pxChange;
        nextPos.wombatRotation = rotation;
        nextPos.wombatUnderground = underground;
        nextPos.wombatAge = age;
        wombatsOnDisplay[index].positionBuffer.append(nextPos);
        //console->print("addingPosToWombatBuffer index: "+QString::number(index)+ " wombatId: "+QString::number(wombatsOnDisplay.value(index).wombatId));
        //console->print("addingPosToWombatBuffer: "+QString::number(wombatsOnDisplay.value(index).positionBuffer.length()));
    }else{
        console->print("Error updateWombatPathSequence wombatId: "+QString::number(wombatId));
    }
}

void OpenGL::unloadNextPosFromWombatPosBuffer(){
   /* if(fuckyoucunt>120){
        int msSinceLast = fuckSoMuch.restart();
        int sSinceLast = msSinceLast / 1000;
        console->print("time since last 120 OPENGL update: "+QString::number(msSinceLast)+" ms "+QString::number(sSinceLast)+" seconds");

        fuckyoucunt = 0;
        int wombatIndex = -1;
        for (int i=0;i<wombatsOnDisplay.length();i++){
            if (wombatsOnDisplay.value(i).wombatId == 208){
                wombatIndex = i;
            }
        }

        if(wombatIndex != -1){
            console->print("imogen position buffer length; "+QString::number(wombatsOnDisplay.value(wombatIndex).positionBuffer.length()));
        }else{
            console->print("can't find wombatId 208 in wombatsOnDisplay");
        }

    }else{
        fuckyoucunt++;
    }*/

    for (int i=0;i<wombatsOnDisplay.length();i++){
        if(wombatsOnDisplay.value(i).moving){
            if(wombatsOnDisplay.value(i).positionBuffer.length()<=0){
                wombatsOnDisplay[i].moving = false;
                gui->controller->setMovingInOpenGl(wombatsOnDisplay[i].wombatId,false);
            }else{
                wombatPosBufferStruct nextPos = wombatsOnDisplay[i].positionBuffer.takeFirst();
                updateWombatPos(i,nextPos.pxChange,nextPos.wombatRotation,nextPos.wombatUnderground,nextPos.wombatAge);
                //console->print("unloading pos from buffer: "+QString::number(pxChange.x())+","+QString::number(pxChange.y()));
            }
        }else{
            if(wombatsOnDisplay.value(i).positionBuffer.length()>0){
                if(!wombatsOnDisplay.value(i).startTimerCalled){
                    wombatsOnDisplay[i].startTimerCalled = true;
                    int id = wombatsOnDisplay[i].wombatId;
                    OpenGL *openGLPointer = this;
                    QTimer::singleShot(2000, this, [openGLPointer, id] {
                            int wombatIndex = -1;
                            for (int i=0;i<openGLPointer->wombatsOnDisplay.length();i++){
                                if (openGLPointer->wombatsOnDisplay.value(i).wombatId == id){
                                    wombatIndex = i;
                                }
                            }

                            if(wombatIndex != -1 && wombatIndex != 0){
                                openGLPointer->wombatsOnDisplay[wombatIndex].moving = true;
                                openGLPointer->gui->controller->setMovingInOpenGl(id,true);
                                openGLPointer->wombatsOnDisplay[wombatIndex].startTimerCalled = false;
                            }
                    });
                }
            }
        }
    }
}


void OpenGL::updateWombatPos(int index, QPoint pxChange, qreal rotation, bool underground, int age){
/*
    //Convert pixels to GL pos
    glWombatStruct wombat;
    float factor = (2.0f/128.0f);
    wombat.glPos = QPointF((float)(tilePos.x() * factor), (float)(tilePos.y()*factor));
    wombat.rotation = rotation;
    wombat.underground = underground;
    wombat.age = age;
*/
    if (index>=0 && index < wombatsOnDisplay.length()){
        if(wombatsOnDisplay[index].age != age){
            updateWombatSize(index,age);
        }

        float factor;
        if (age>1){
            factor = (2.0f/128.0f); //Grown wombat size
        }else if (age==1){
            factor = (2.0f/104.0f);
        }else{
            factor = (2.0f/80.0f); // Joey wombat size
        }

        wombatsOnDisplay[index].glPos += QPointF((float)(pxChange.x() * factor), (float)(pxChange.y()*factor));
        wombatsOnDisplay[index].rotation = rotation;
        wombatsOnDisplay[index].underground = underground;
        wombatsOnDisplay[index].age = age;
    }else{
        console->print("Error updateWombatPosition");
    }
}

void OpenGL::updateWombatSize(int index, int age){
    if (age==1){
        QPointF pos = wombatsOnDisplay[index].glPos;
        pos = pos/(2.0f/80.0f);
        pos = pos*(2.0f/104.0f);
        wombatsOnDisplay[index].glPos = pos;
        wombatsOnDisplay[index].age = age;
    }

    if (age==2){
        QPointF pos = wombatsOnDisplay[index].glPos;
        pos = pos/(2.0f/104.0f);
        pos = pos*(2.0f/128.0f);
        wombatsOnDisplay[index].glPos = pos;
        wombatsOnDisplay[index].age = age;
    }
}

void OpenGL::setWombatTask(int index, WombatAITask currentTask){ //Remember: Task in openGL is only used to set the correct texture/icon, nothing else
    if(index>=0 && index<wombatsOnDisplay.length()){
        wombatsOnDisplay[index].currentTask = currentTask;
    }else{
        console->print("Error setWombatTask");
    }
}

void OpenGL::setWombatEmoticon(int index, WombatEmoticon emoticon){
    if(index>=0 && index<wombatsOnDisplay.length()){
        wombatsOnDisplay[index].emoticon = emoticon;
    }else{
        console->print("Error setWombatEmoticion");
    }
}

void OpenGL::removeWombatFromDisplay(int wombatId){
    int index = None;
     for (int i=0;i<wombatsOnDisplay.length();i++){
        if(wombatsOnDisplay.value(i).wombatId == wombatId){
            index = i;
            break;
        }
     }

    if(index!=None){
        //console->print("Removing wombat id "+QString::number(wombatsOnDisplay.value(index).wombatId)+ " from OpenGL display");
        //console->print("brefore remove id wombatsondispaly in opengl: "+QString::number(wombatsOnDisplay.length()));
        //for (int i=0;i<gui->openGL->wombatsOnDisplay.length();i++){
        //    console->print("wombatId: "+QString::number(wombatsOnDisplay.value(i).wombatId));
        //}
        wombatsOnDisplay.removeAt(index);
        //console->print("after remove wombatsondispaly in opengl: "+QString::number(wombatsOnDisplay.length()));
        //for (int i=0;i<gui->openGL->wombatsOnDisplay.length();i++){
        //    console->print("wombatId: "+QString::number(wombatsOnDisplay.value(i).wombatId));
        //}
    }else{
        console->print("Error removeWombatFromDisplay");
    }
}

void OpenGL::setSelectedWombatVisibility(bool b){
    //console->print("Setting selected wombat visibility in openl: "+QString::number(b));
    selectedWombatVisibility = b;
}

void OpenGL::setSelectedWombatId(int wombatId){
    wombatsOnDisplay[0].wombatId = wombatId;
}

void OpenGL::setSelectedWombatRotation(qreal rotation){
    wombatsOnDisplay[0].rotation = rotation;
}

int OpenGL::getWombatOnDisplayIndex(int wombatId){
    int index = None;
    for (int i=0;i<wombatsOnDisplay.length();i++){
        if (wombatsOnDisplay.value(i).wombatId == wombatId){
            index = i;
            return index;
        }
    }

    return index;
}


