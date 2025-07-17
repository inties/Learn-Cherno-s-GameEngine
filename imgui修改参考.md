void setImgui(GLFWwindow* window)
{
	bool Imgui;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Settings", &Imgui, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	ImGui::Text("SSDO Settings:");
	ImGui::SliderInt("Size of Kernel", &kernelSize, 1, 500);
	ImGui::SliderFloat("Radius", &radius, 0.0f, 3.0f, "%.1f");

	ImGui::Text("Light Settings:");
	ImGui::Text("Position:");
	ImGui::SliderFloat("Light Position.x", &lightXYZ[0], -10.0f, 10.0f, "%.3f");
	ImGui::SliderFloat("Light Position.y", &lightXYZ[1], -10.0f, 10.0f, "%.3f");
	ImGui::SliderFloat("Light Position.z", &lightXYZ[2], -10.0f, 10.0f, "%.3f");
	ImGui::Text("Color:");
	ImGui::ColorEdit3("Light Color", lightColor);

	ImGui::Text("Model Settings:");
	const char* items[] = { "Dragon", "Buddha","Arma","Block","Bunny","Dinosaur"
,"Feisar","Horse","Kitten","Rocker" };
	ImGui::Combo("Model", &modelType, items, IM_ARRAYSIZE(items));
	ImGui::SliderFloat("Model Angel-x", &modelAngle1, -180.0f, 180.0f, "angle = %.1f");
	ImGui::SliderFloat("Model Angel-y", &modelAngle2, -180.0f, 180.0f, "angle = %.1f");
	ImGui::SliderFloat("Model Angel-z", &modelAngle3, -180.0f, 180.0f, "angle = %.1f");

	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Options") == 1)
	{
		if (ImGui::MenuItem("MC Mode", NULL))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			MC_mode = 1;
		}
		if (ImGui::MenuItem("Quit", NULL))
			exit(0);
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}