#include "ofApp.h"

#include "tiny_obj_loader.h"

//--------------------------------------------------------------
void ofApp::setup(){
	_imgui.setup();

	_camera.setNearClip(0.1f);
	_camera.setFarClip(100.0f);
	_camera.setDistance(5.0f);


	std::string inputfile = ofToDataPath("bunny.obj");
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	std::vector<ofVec3f> vertices;
	std::vector<ofVec3f> normals;
	std::vector<size_t> indices;

	int vertexCount = attrib.vertices.size() / 3;
	for (int i = 0; i < vertexCount; ++i) {
		float vx = attrib.vertices[3 * i + 0];
		float vy = attrib.vertices[3 * i + 1];
		float vz = attrib.vertices[3 * i + 2];
		vertices.emplace_back(vx, vy, vz);

		float nx = attrib.normals[3 * i + 0];
		float ny = attrib.normals[3 * i + 1];
		float nz = attrib.normals[3 * i + 2];
		normals.emplace_back(nx, ny, nz);
	}

	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			assert(fv == 3);
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				indices.push_back(idx.vertex_index);
			}
			index_offset += fv;
		}
	}

	_mesh.addVertices(vertices);
	_mesh.addNormals(normals);
	_mesh.addIndices(indices);

	_shader.load("shader.vert", "shader.frag");


	glGenTextures(1, &_fitLookup);
	// 64x64 table
	const float data[] = {
#include "fit.inl"
	};

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _fitLookup);
	glTexStorage2D(GL_TEXTURE_2D,
		1,
		GL_RGBA32F,
		64,
		64);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 64, GL_RGBA, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	{
		_plane = ofMesh::plane(5, 5);
		auto &vs = _plane.getVertices();
		for (int i = 0; i < vs.size(); ++i) {
			vs[i].z -= 1.0f;
		}
	}

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofEnableDepthTest();

	ofClear(0);
	_camera.begin();
	ofPushMatrix();
	ofRotateZ(90.0f);
	ofSetColor(255);
	ofDrawGridPlane(1.0f);
	ofPopMatrix();

	ofPushMatrix();
	ofDrawAxis(50);
	ofPopMatrix();

	ofSetColor(255);
	ofDrawSphere(_sphere_position, _sphere_radius);

	auto camera_position = _camera.getPosition();

	_shader.begin();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _fitLookup);

	_shader.setUniform3f("u_radiance", ofVec3f(_radiance));

	_shader.setUniform3f("u_camera_position", camera_position);
	_shader.setUniform3f("u_sphere_position", _sphere_position);
	_shader.setUniform1f("u_sphere_radius", _sphere_radius);
	
	_shader.setUniform1f("u_rouphness", _rouphness);

	_shader.setUniform1i("u_fitLookup", 0);
	
	_mesh.draw();

	// °
	_plane.draw();

	glBindTexture(GL_TEXTURE_2D, 0);

	_shader.end();
	
	// _mesh.drawWireframe();

	_camera.end();


	ofDisableDepthTest();
	ofSetColor(255);

	_imgui.begin();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ofVec4f(0.2f, 0.2f, 0.5f, 0.5f));
	ImGui::SetNextWindowPos(ofVec2f(500, 30), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ofVec2f(500, 600), ImGuiSetCond_Once);

	ImGui::Begin("Config Panel");
	ImGui::Text("fps: %.2f", ofGetFrameRate());
	
	ImGui::SliderFloat("radiance", &_radiance, 0, 20);
	ImGui::SliderFloat("rouphness", &_rouphness, 0, 1);
	ImGui::SliderFloat3("sphere p", _sphere_position.getPtr(), -5, 5);
	ImGui::SliderFloat("sphere r", &_sphere_radius, 0.01f, 5.0f);

	auto wp = ImGui::GetWindowPos();
	auto ws = ImGui::GetWindowSize();
	ofRectangle win(wp.x, wp.y, ws.x, ws.y);

	ImGui::End();
	ImGui::PopStyleColor();

	_imgui.end();

	if (win.inside(ofGetMouseX(), ofGetMouseY())) {
		_camera.disableMouseInput();
	}
	else {
		_camera.enableMouseInput();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
