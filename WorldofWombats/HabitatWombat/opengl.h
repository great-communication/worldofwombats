//The OpenGLWidget that displays the world
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QTextEdit>
#include <QOpenGLShaderProgram>
#include "global.h"
#include <QTime> //remove this!

class GUI;
class Console;

class OpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGL(QWidget *parent = 0);
    ~OpenGL();
    GUI *gui;
    Console *console;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;    
    struct glWombatStruct {
        int wombatId;
        QPointF glPos;
        QPoint originalOffset;
        qreal rotation;
        bool underground;
        int age;
        WombatAITask currentTask;
        WombatEmoticon emoticon;
        bool moving;
        QVector<wombatPosBufferStruct> positionBuffer;
        bool startTimerCalled;
    };
    QVector<glWombatStruct> wombatsOnDisplay;
    bool selectedWombatVisibility;
    bool backgroundImageUnderground; //If the backgroundImage is displaying the underground or not // Needed because there is a delay between changing the underground variable in gui, and displaying the new background image prepared by the backend thread
    void updateBackgroundSize(QSize backgroundSize);
    void updateBackgroundTextureSize(const QImage &backgroundImage);
    void setup(QSize backgroundSize);
    void removeWombatFromDisplay(int wombatId);
    void addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask);
    void updateWombatPos(int index, QPoint tilePos, qreal rotation, bool underground, int age);
    void updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground);
    void updateTexturePart(const QImage &backgroundImage,QPoint visibleTilePos);
    void resetOriginalOffset();
    void setSelectedWombatVisibility(bool b);
    void setSelectedWombatId(int wombatId);
    void setSelectedWombatRotation(qreal rotation);
    void updateWorldTranslate();
    int getWombatOnDisplayIndex(int wombatId);
    void updateWombatSize(int index, int age);
    void setWombatTask(int index, WombatAITask currentTask);
    void setWombatEmoticon(int index, WombatEmoticon emoticon);
    void addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age);
    int bestFuckSoFar;

signals:
    void clicked();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    void makeObject();
    QColor clearColor;   
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;
    GLuint tex;
    GLuint wombatTex;
    GLuint wombatSleepingTex;
    GLuint exclamationEmoticon;
    GLuint sleepingEmoticon;
    float xFct, yFct;
    QMatrix4x4 worldTranslate;  //This is how much the world has moved compared to worldPosition
    QMatrix4x4 worldProjection;    
    //QMatrix4x4 projectionOffset;
    QMatrix4x4 wombatTranslate;    
    QMatrix4x4 wombatProjection;
    QMatrix4x4 wombatRotation;    
    float movedXTotal, movedYTotal;
    int movedXTotalInPixels, movedYTotalInPixels;
    QPointF originalWindowSize;
    QSize backgroundSize;
    void updateWorldTranslate2();
    void drawWombat(int i, float depthAdd);
    void drawLabel(int i, int age);
    void rotateWombat(int index, bool up, bool down, bool left, bool right);
    void unloadNextPosFromWombatPosBuffer();
    int fuckyoucunt;
    QTime fuckSoMuch;
    QTime fuckSoMuch72;

};

#endif
