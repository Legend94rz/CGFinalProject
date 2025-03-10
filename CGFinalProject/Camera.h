#pragma once
#include <D3DX10.h>
class Camera
{
public:
	Camera();
	~Camera();

	D3DXVECTOR3& position();

	D3DXMATRIX view()const;
	D3DXMATRIX proj()const;

	void setLens(float fovY, float aspect, float zn, float zf);

	void strafe(float d);
	void walk(float d);

	void pitch(float angle);
	void rotateY(float angle);

	void rebuildView();

private:
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mRight;
	D3DXVECTOR3 mUp;
	D3DXVECTOR3 mLook;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
};

Camera& GetCamera();
